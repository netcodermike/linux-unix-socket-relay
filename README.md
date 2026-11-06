# Linux Unix Socket Relay

Forward one bidirectional Unix socket stream at a time, preserve half-closes, and report byte totals.

[![Build and verify](https://github.com/netcodermike/linux-unix-socket-relay/actions/workflows/verify.yml/badge.svg)](https://github.com/netcodermike/linux-unix-socket-relay/actions/workflows/verify.yml)

**Language:** C · **Platform:** Linux · **App type:** Linux local IPC relay and diagnostics

First-release learning implementation, with explicit scope below. This is one independently runnable app, not a placeholder in a combined repository.

## Features

- Unix stream listener forwarding to an upstream socket.
- One active connection at a time with binary-safe forwarding.
- Half-close propagation and per-direction byte totals.
- Private socket permissions, refusal to overwrite paths, and signal cleanup.

## Requirements

Linux Unix-domain stream sockets, a C11 compiler, make, and Python 3 for integration tests. No root privileges or network listener.

## Quick start

```sh
git clone https://github.com/netcodermike/linux-unix-socket-relay.git
cd linux-unix-socket-relay
make
# Start an existing Unix stream server first, then:
./relay /tmp/my-private-dir/relay.sock /tmp/my-private-dir/upstream.sock
# Use an existing local client against relay.sock.
# The listener parent directory must already exist.
```

Commands run from the repository root. Lines marked for a particular operating system apply only there. Language tools are prerequisites, not installed by the app. Close desktop windows normally; terminal/server apps stop with Ctrl+C unless their usage notes say otherwise.

## Example workflow

Run make test for a complete local echo setup. For manual use, create a private temporary directory, start a Unix stream service, run the relay, and point a local client at its listener. Close the client’s write half and confirm the upstream response still arrives.

## Architecture, configuration, and data

`relay.c` binds a new filesystem socket under umask 0077, accepts a client, connects upstream, and polls both peers. Data is copied in 16 KiB chunks with partial-write handling and three-second send timeouts. EOF shuts down only the opposite write half, allowing the remaining direction to drain. SIGINT/SIGTERM stop the loops; cleanup removes the listener only if its inode/device still match the socket created by this process. `tests/integration.py` supplies a real Unix echo server and checks a 102,400-byte binary round trip, EOF, byte counts, and removal. No traffic leaves Unix-domain sockets.

## Verification

Build with the quick-start command where needed, then run:

```sh
make test
```

Manual acceptance: Run the integration test, try an unavailable upstream, confirm an existing path is preserved, exercise client half-close, and stop during an idle session.

See [validation record](docs/VALIDATION.md) for observed checks and their limits. CI results verify only what the workflow runs; they do not establish native GUI correctness or production readiness.

## Limitations and next steps

Sequential sessions, no TLS, multiplexing, protocol parsing, authentication layer beyond filesystem permissions, FD passing, abstract sockets, or replay logs. Bounded blocking writes can drop a session when a slow peer exceeds three seconds; this is not a high-throughput nonblocking proxy. Idle sessions wait until data/EOF/signal. Upstream services must be trusted for your intended use. Signal failures or crashes can leave a stale socket, which must be inspected/removed manually.

## Troubleshooting

Existing listener paths are intentionally rejected; the app never preemptively unlinks them. Use a private short path within the Unix socket path limit. Upstream connection errors close that client and continue listening. If a client stalls, inspect its send/receive pattern and the three-second timeout. Ctrl+C or SIGTERM cleans up the owned listener.

## Source map

- `Makefile`
- `relay.c`
- `tests/integration.py`

## Development and license

Read [contributing](CONTRIBUTING.md) before changing the app, and [security/privacy](SECURITY.md) before supplying private data. Use synthetic fixtures for tests. No telemetry, paid API key, or account is required by the app itself. Dependencies, when present, are stated in Requirements.

[MIT license](LICENSE), copyright 2026 netcodermike.
