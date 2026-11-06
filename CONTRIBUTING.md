# Contributing

## Setup

Follow README.md for the exact language/platform tools, launch command, architecture, and stored data. Commands run from the repository root. Generated files and local stores should remain out of Git.

## A focused change

1. Explain the concrete behavior and include a small synthetic reproduction.
2. Keep the implementation limited to that behavior and preserve documented data formats where possible.
3. Build the app and run `make test`.
4. Exercise the applicable manual acceptance checks from README.md on the target platform.
5. Update usage, limitations, examples, and validation notes when behavior changes.
6. Open a pull request stating actual checks passed and checks not run.

## Bug reports

Include OS, runtime/compiler versions, exact command/steps, expected result, actual result, and a sanitized minimal input. A successful compilation does not substitute for a GUI/device acceptance test. Avoid attaching local stores, secrets, build output folders, or unrelated files.
