# Lab 10 Add the trace system call to xv6

Student: Cimendur Rustem, FAF-243.

## Confirmed starting environment

Source tree: `/home/vboxuser/xv6-riscv`, revision `06aad25`.
Existing edits from previous laboratories must be preserved.
The system call number 22 belongs to sync; trace must use number 23.
The kernel print interface is printk. README is 2441 bytes.

## Materials

`user/trace.c` is the user program from the handout. Build it inside the xv6
tree after adding its declaration, syscall number, stub entry and UPROGS entry.

The baseline boot and Part 1 checkpoint have been verified from the VM
screenshots. trace uses number 23, its generated user stub contains ecall,
and the unfinished kernel reports `3 trace: unknown sys call 23` followed
by `trace: failed`, as required at this checkpoint.

`apply_kernel.py` applies the Part 2 kernel changes from the xv6 source root.
It validates each source anchor before writing the four kernel files. Besides
the handout changes, it initializes the mask to zero when a process slot is
allocated, preventing stale tracing settings after slot reuse.

The kernel implementation and all Part 3 mask experiments have been verified.
The forkforkfork test printed OK and ALL TESTS PASSED. The report is
`../lab10_report.docx`; `report.md` records the observations, and `notes.txt`
contains the four Observe answers. Original evidence is in `screenshots/`.

`kernel/` and `user/` contain the modified source files. These were reproduced
from the official xv6 revision and checked against the Git blob hashes
visible in the VM screenshots:

| File | Verified Git blob prefix |
| --- | --- |
| kernel/proc.c | 2dd5d14 |
| kernel/proc.h | e3af86f |
| kernel/syscall.c | 220f94f |
| kernel/sysproc.c | f3b5d40 |
| kernel/syscall.h | 57e1feb |
| user/user.h | b8002ab |
| user/usys.pl | b73860c |

Upstream source: https://github.com/mit-pdos/xv6-riscv/tree/06aad25.
The copied upstream files retain their license in `LICENSE.xv6`.

## Reproduce in a fresh xv6 checkout

`trace.patch` contains the complete trace implementation, including its user
program and Makefile entry. The patch does not add dependencies on earlier
laboratory programs. It was checked against the clean upstream revision.

```bash
git clone https://github.com/mit-pdos/xv6-riscv.git xv6-lab10
cd xv6-lab10
git checkout 06aad25
git apply /absolute/path/to/operating-systems-labs/lab10/trace.patch
make qemu
```

Inside xv6:

```text
trace 32 grep hello README
trace 2147483647 grep hello README
trace 2 usertests forkforkfork
trace 65536 echo hi
trace 2129920 grep hello README
echo LAB10_UNTRACED_OK
trace 0 echo LAB10_MASK_ZERO_OK
```

The VM's Makefile also references earlier laboratory programs. When committing
that existing tree, include their source files so a checkout remains buildable.
