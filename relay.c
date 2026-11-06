#define _POSIX_C_SOURCE 200809L
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <poll.h>
#include <signal.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
static volatile sig_atomic_t running=1;
static void stop(int n){(void)n;running=0;}
static int connect_to(const char *path){
 struct sockaddr_un address;memset(&address,0,sizeof address);address.sun_family=AF_UNIX;
 if(strlen(path)>=sizeof address.sun_path){errno=ENAMETOOLONG;return -1;}strcpy(address.sun_path,path);
 int fd=socket(AF_UNIX,SOCK_STREAM,0);if(fd<0)return -1;struct timeval timeout={3,0};if(setsockopt(fd,SOL_SOCKET,SO_SNDTIMEO,&timeout,sizeof timeout)<0){close(fd);return -1;}if(connect(fd,(struct sockaddr*)&address,sizeof address)<0){close(fd);return -1;}return fd;
}
static int write_all(int fd,const char *data,size_t size){while(size){ssize_t n=send(fd,data,size,MSG_NOSIGNAL);if(n<=0){if(n<0&&errno==EINTR&&running)continue;return -1;}data+=n;size-=(size_t)n;}return 0;}
static int relay(int client,int upstream){
 struct pollfd peers[2]={{client,POLLIN,0},{upstream,POLLIN,0}};int active[2]={1,1};unsigned long long counts[2]={0,0};char buffer[16384];int result=0;
 while(running&&(active[0]||active[1])){
  for(int i=0;i<2;i++)peers[i].fd=active[i]?(i?upstream:client):-1;
  int ready=poll(peers,2,1000);if(ready<0){if(errno==EINTR)continue;result=-1;break;}if(!ready)continue;
  for(int i=0;i<2;i++)if(active[i]&&(peers[i].revents&(POLLIN|POLLHUP|POLLERR))){
   ssize_t n=recv(peers[i].fd,buffer,sizeof buffer,0);if(n>0){counts[i]+=(unsigned long long)n;if(write_all(i?client:upstream,buffer,(size_t)n)<0){result=-1;active[0]=active[1]=0;break;}}
   else if(n==0){active[i]=0;shutdown(i?client:upstream,SHUT_WR);}else if(errno!=EINTR){result=-1;active[i]=0;shutdown(i?client:upstream,SHUT_WR);}
  }
 }
 fprintf(stderr,"session bytes client->upstream=%llu upstream->client=%llu\n",counts[0],counts[1]);return result;
}
int main(int argc,char **argv){
 if(argc!=3){fprintf(stderr,"Usage: relay LISTEN_SOCKET UPSTREAM_SOCKET\nOne local stream session at a time. Existing listener paths are never removed.\n");return 2;}
 struct sockaddr_un address;memset(&address,0,sizeof address);address.sun_family=AF_UNIX;if(strlen(argv[1])>=sizeof address.sun_path){fprintf(stderr,"Listener path too long\n");return 2;}strcpy(address.sun_path,argv[1]);
 umask(0077);int server=socket(AF_UNIX,SOCK_STREAM,0);if(server<0){perror("socket");return 1;}
 if(bind(server,(struct sockaddr*)&address,sizeof address)<0){perror("bind (existing path is not overwritten)");close(server);return 1;}
 struct stat owned;if(lstat(argv[1],&owned)<0){close(server);return 1;}
 if(listen(server,8)<0){perror("listen");close(server);unlink(argv[1]);return 1;}
 struct sigaction action;memset(&action,0,sizeof action);action.sa_handler=stop;sigemptyset(&action.sa_mask);sigaction(SIGINT,&action,NULL);sigaction(SIGTERM,&action,NULL);signal(SIGPIPE,SIG_IGN);
 fprintf(stderr,"Listening %s -> %s\n",argv[1],argv[2]);
 while(running){struct pollfd p={server,POLLIN,0};int n=poll(&p,1,500);if(n<0){if(errno==EINTR)continue;perror("poll");break;}if(!n)continue;int client=accept(server,NULL,NULL);if(client<0){if(errno==EINTR)continue;perror("accept");break;}
  struct timeval timeout={3,0};if(setsockopt(client,SOL_SOCKET,SO_SNDTIMEO,&timeout,sizeof timeout)<0){perror("client timeout");close(client);continue;}
  int upstream=connect_to(argv[2]);if(upstream<0){perror("connect upstream");close(client);continue;}
  relay(client,upstream);close(upstream);close(client);
 }
 close(server);struct stat current;if(lstat(argv[1],&current)==0&&current.st_dev==owned.st_dev&&current.st_ino==owned.st_ino)unlink(argv[1]);return 0;
}
