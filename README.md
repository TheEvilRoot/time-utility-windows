# Time

Time is utility program that can be used to track execution time of some executable. Inspired by unix's `time`.

## Usage

```bash

% time [-h] executable [-- [...executable args]]

```

## Parameters

#### -h - use suffixes

When `-h` is specified, execution time will be displayed in user-friendly form (like `1.2s` or `24.1m`).

#### -- - executable arguments delimiter

All arguments after `--` will be passed as executable command line arguments.

