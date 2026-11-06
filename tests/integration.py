import pathlib,socket,subprocess,tempfile,threading
binary=str(pathlib.Path('relay').resolve())
with tempfile.TemporaryDirectory(prefix='relay-',dir='/tmp')as tmp:
    root=pathlib.Path(tmp);upstream=str(root/'up.sock');listener=str(root/'relay.sock');errors=[]
    def echo():
        try:
            with socket.socket(socket.AF_UNIX)as server:
                server.bind(upstream);server.listen(1)
                with server.accept()[0]as peer:
                    peer.settimeout(5)
                    while True:
                        b=peer.recv(65536)
                        if not b:break
                        peer.sendall(b)
        except Exception as e:errors.append(e)
    thread=threading.Thread(target=echo,daemon=True);thread.start()
    import time
    for _ in range(100):
        if pathlib.Path(upstream).exists():break
        time.sleep(.01)
    p=subprocess.Popen([binary,listener,upstream],stdout=subprocess.PIPE,stderr=subprocess.PIPE,text=True)
    try:
        assert p.stderr.readline().startswith('Listening ')
        data=bytes(range(256))*400
        with socket.socket(socket.AF_UNIX)as client:
            client.settimeout(5);client.connect(listener)
            # Concurrent receive avoids deliberately filling both blocking peers.
            response=bytearray()
            def receive():
                while True:
                    b=client.recv(65536)
                    if not b:break
                    response.extend(b)
            reader=threading.Thread(target=receive);reader.start();client.sendall(data);client.shutdown(socket.SHUT_WR);reader.join(5);assert not reader.is_alive();assert response==data
        thread.join(5);assert not errors,errors
    finally:p.terminate();out,err=p.communicate(timeout=5)
    assert p.returncode==0 and not pathlib.Path(listener).exists()
    assert 'client->upstream=102400'in err
print('Binary forwarding, bidirectional flow, half-close, byte counts, and cleanup passed')
