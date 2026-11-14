# Lab 9 Watching System Calls with strace

Cimendur Rustem, FAF-243. Experiments performed on 14 November 2026.

## Environment

Ubuntu Linux in VirtualBox; vboxuser on host rustem.
Working directory: `/home/vboxuser/os-lab9/run-20261114-201149`.
Kernel `7.0.0-38-generic`, GCC `15.2.0`, strace `6.19`.

## Part 1 Trace a real command

```bash
strace -f -e trace=openat,read,write,close -o ls.trace ls -l
cat ls.trace
grep -Ec '(openat|read|write|close)\(' ls.trace
grep 'write(1,' ls.trace
strace -f -c -o ls-summary.txt ls -l
cat ls-summary.txt
```

The filtered trace contained 86 selected calls. In the repeated trace,
process 6099 used `write(1, ..., 130) = 130`. The full summary recorded
266 calls, including 9 errors. It showed 31 openat, 20 read, 34 close and
1 write calls; these sum to 86. Counts depend on directory contents,
environment and trace options, and are measurements from these runs.

### Observe Which call prints and to which descriptor

The command uses write on descriptor 1, standard output. That descriptor was
connected to the terminal. Shared library loading, account lookups and other
supporting operations also appear in the trace.

## Part 2 File operations in a C program

```bash
gcc -Wall -Wextra -o syscalls syscalls.c
./syscalls
printf 'Program exit status: %s\n' "$?"
cat note.txt
wc -c note.txt
strace -e trace=openat,write,read,close -o syscalls.trace ./syscalls
cat syscalls.trace
```

Compilation produced no visible warnings. The output reported descriptor 3,
14 bytes written and 14 bytes read. The program returned status 0.
note.txt contained `hello, kernel` followed by a newline; wc confirmed 14 bytes.

The trace showed openat on note.txt with creation/truncation flags, write on
fd 3 returning 14, close, another openat returning 3, read requesting at most
63 bytes and returning 14, and close. printf diagnostics used write on fd 1.
The source's open library interface was implemented using the openat syscall.

### Observe Why the descriptor is three

Descriptors 0, 1 and 2 are already occupied by standard input, standard output
and standard error. Descriptor 3 is the lowest available descriptor. After
close releases it, a later open can reuse it.

## Conclusion

The trace exposes syscall arguments, return values and descriptor reuse.
The 14-byte write, file size and read result agree. Reading fewer than the
63 requested bytes is expected for a file that contains only 14 bytes.

Screenshots and captions are included in `../lab9_report.docx`.
