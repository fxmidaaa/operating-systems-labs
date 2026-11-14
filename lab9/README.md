# Lab 9 Watching System Calls with strace

Student: Cimendur Rustem, FAF-243.

## Materials

`syscalls.c` follows the C program supplied in the laboratory handout.
Build and run it on Linux:

```bash
gcc -Wall -Wextra -o syscalls syscalls.c
./syscalls
strace -e trace=openat,write,read,close ./syscalls
```

The sample assumes successful file reads and writes after its initial open.
Use it in a writable experiment directory, as in the handout.

## Recorded results

Experiments performed on 14 November 2026, in Ubuntu Linux inside VirtualBox.
Account: vboxuser on rustem. Directory:
`/home/vboxuser/os-lab9/run-20261114-201149`.
Kernel: `7.0.0-38-generic`; GCC: `15.2.0`; strace: `6.19`.

- The filtered ls trace contains 86 calls to openat, read, write and close.
- The full ls summary contains 266 calls and 9 syscall errors.
- The recorded ls output uses `write(1, ..., 130) = 130`.
- syscalls.c exits with status 0 and writes and reads 14 bytes through fd 3.
- note.txt is 14 bytes, including its trailing newline.

The report is `../lab9_report.docx`. `report.md` contains the commands and
Observe answers. `screenshots/` contains the original execution evidence.
`commands.sh` repeats the experiments in a new directory.

The supplied two-page handout lists Part 3, Count one kind of call, in its
overview but contains no Part 3 instructions. The report covers the printed
Parts 1 and 2, both Observe questions, and the additional count summary.
