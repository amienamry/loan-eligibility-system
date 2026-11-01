# Personal Loan Eligibility System

A small C99 command-line application that validates customer details, checks
personal loan eligibility, summarizes applications, and writes records to
`loanapplications.txt`.

## Requirements

- A C compiler: GCC or Clang
- GNU Make for the Makefile instructions (commonly available on macOS, Linux,
  and MSYS2)
- On Windows without Make, MinGW-w64 GCC on `PATH` to use `build.bat`

No third-party libraries are required.

## Build and run

### macOS and Linux

From the project directory:

```sh
make
make run
```

Or compile and run directly:

```sh
cc -std=c99 -Wall -Wextra -Wpedantic main.c -o loan-eligibility
./loan-eligibility
```

To remove the generated executable:

```sh
make clean
```

You can select a compiler or add compiler options when building:

```sh
make CC=clang
make CFLAGS="-std=c99 -Wall -Wextra -Wpedantic -O2"
```

### Windows

Install MinGW-w64 (for example through MSYS2) and make sure `gcc` is available
on `PATH`. From Command Prompt, run:

```bat
build.bat
```

The script compiles `main.c` to `loan-eligibility.exe` with warnings enabled,
then starts the program. Alternatively, in PowerShell:

```powershell
gcc -std=c99 -Wall -Wextra -Wpedantic main.c -o loan-eligibility.exe
.\loan-eligibility.exe
```

## Using the application

Enter a positive customer ID and follow the prompts. Enter `0` for the
customer ID to finish and show the summary. Income and commitments must be
entered as numbers without an `RM` prefix. Names may contain spaces and are
limited to 49 characters.

The application accepts up to 100 customers. It calculates DTI as
`commitments * 100 / income`; a customer is eligible when income is at least
RM3,000 and DTI is no more than 40%. Zero-income customers are not eligible,
are shown with DTI `N/A`, and count in the average DTI denominator.

When at least one customer has been entered, the program writes all records
to `loanapplications.txt` in the **current working directory**. The file is
overwritten on each run. Names are quoted and escaped as CSV fields. If no
customers are entered, no output file is created.

## Project files

- `main.c` — application source code
- `Makefile` — build, run, and clean targets for Make-compatible environments
- `build.bat` — build and run helper for Windows with MinGW-w64 GCC
