# Lab 8 xv6 processes and pipes

Cimendur Rustem, FAF-243.

The [Word report](../lab8_report.docx) contains the Ubuntu VM and xv6 screenshots, experiments and conclusions. The source files below reproduce the listings and variants shown in that report.

| File | Purpose |
| --- | --- |
| [user/run.c](user/run.c) | Final version: fork a child, execute echo and collect the child status. |
| [user/pingpong.c](user/pingpong.c) | Exchange one byte in both directions through two pipes. |
| [user/primes.c](user/primes.c) | Optional filter pipeline for prime numbers from 2 through 35. |
| [experiments/run_ls.c](experiments/run_ls.c) | Part 1 experiment in which the child executes ls. |
| [experiments/run_bad.c](experiments/run_bad.c) | Part 1 experiment in which exec fails and the child exits with status 1. |
| [Makefile](Makefile) | Complete xv6 Makefile with run, pingpong and primes in UPROGS. |
| [notes.md](notes.md) | All three Part 1 and both Part 2 Observe answers. |

## Build in the existing Ubuntu xv6 tree

From this repository, copy the three final programs into the xv6 source tree:

```bash
cp lab8/user/*.c ~/xv6-riscv/user/
cd ~/xv6-riscv
```

Add the following entries to the existing `UPROGS` list in the xv6 Makefile, keeping the entries from earlier laboratories. Each line starts with a tab and ends with a backslash.

```makefile
	$U/_primes\
	$U/_pingpong\
	$U/_run\
```

Build and boot from the xv6 directory:

```bash
make kernel/kernel fs.img
make qemu
```

Inside xv6, enter one command at a time:

```text
run
pingpong
pingpong
pingpong
primes
```

`run` prints the echo greeting and the child's status 0. Each `pingpong` run prints the child's `received ping` before the parent's `received pong`; the actual PIDs depend on previous commands. `primes` prints exactly 2, 3, 5, 7, 11, 13, 17, 19, 23, 29 and 31, one per line with the prefix `prime`. The shell prompt returns after each program finishes. Exit QEMU with Ctrl+A, then X.

## Reproduce the exec experiments

The final `user/run.c` remains the echo version. To run either experimental variant alongside it, copy the variant into the xv6 `user/` directory and add its own UPROGS entry:

```bash
# Run from this repository.
cp lab8/experiments/run_ls.c lab8/experiments/run_bad.c ~/xv6-riscv/user/
```

```makefile
	$U/_run_ls\
	$U/_run_bad\
```

Rebuild with `make qemu`, then run `run_ls` and `run_bad` inside xv6. The first variant lists files and reports child status 0. The second prints `exec failed` and reports child status 1. The parent still exits with status 0 after reporting its child's result, just as in the report.

## Supplied Makefile

The complete Makefile is based on the official MIT xv6 revision `06aad25c735fd3159bdfae5680be4eab7b1668b2` recorded in the report, with only the three final Lab 8 UPROGS entries added. It is intended for a clean matching xv6 checkout. In an existing tree containing earlier laboratory programs, add the entries as described above.

The upstream Makefile is covered by the accompanying [MIT license](LICENSE.xv6).
