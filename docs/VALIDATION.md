# Validation

Authoring date: November 6, 2026. Host: Windows. Target: Linux.

## Local result

Linux Unix socket runtime unavailable on authoring host. CI compiles with strict warnings and runs real socket integration checks.

## Reproducible checks

The GitHub Actions workflow builds/runs the following on `ubuntu-latest`:

```sh
make
make test
```

## Manual acceptance

Run the integration test, try an unavailable upstream, confirm an existing path is preserved, exercise client half-close, and stop during an idle session.

A workflow file is not evidence of a passing run. Observed CI run links and source commits are recorded after publication. Native GUI, sound, sleep/resume, and platform permissions require direct target-platform acceptance even when model tests pass.

## Observed CI result

Passed: [target-platform build/check](https://github.com/netcodermike/linux-unix-socket-relay/actions/runs/42204121065). Source commit: `e6545493c5ddc2cee869b5d4a8d17f3d2aaf1a77`. Later documentation-only changes do not alter the tested implementation. The reproducible commands above identify test scope; native GUI/audio/permissions are not implied by compilation.
