# Operating Systems Labs

Laboratory reports and source code for Labs 1–8 of the Operating Systems course at the Technical University of Moldova.

**Student:** Cimendur Rustem, FAF-243

| Lab | Topic | Report | Source code |
| --- | --- | --- | --- |
| 1 | Linux resource management | [Report](lab1_report.docx) | — |
| 2 | Building and extending xv6 | [Report](lab2_report.docx) | [sleep.c](lab2/user/sleep.c) |
| 3 | Bash, pipes and text filters | [Report](lab3_report.docx) | [report.sh](lab3/report.sh) |
| 4 | Bash and xv6 shell comparison | [Report](lab4_report.docx) | [args.c](lab4/user/args.c) |
| 5 | Linux file systems, links, permissions and storage | [Report](lab5_report.docx) | — |
| 6 | xv6 file system, metadata and recursive search | [Report](lab6_report.docx) | [mystat.c](lab6/user/mystat.c), [find.c](lab6/user/find.c) |
| 7 | Linux processes, redirection and pipes | [Report](lab7_report.docx) | [C programs and experiment notes](lab7/) |
| 8 | xv6 processes, pingpong and a prime filtering pipeline | [Report](lab8_report.docx) | [Programs, Makefile and build instructions](lab8/), [Observe answers](lab8/notes.md) |

## Environment

The experiments use Ubuntu Linux in VirtualBox. The xv6 labs run the RISC-V version of xv6 in QEMU, using a RISC-V GCC cross compiler. The reports record the environment, commands, observations and screenshots for each lab.

## Source code

- **Lab 2:** An xv6 `sleep` program that waits for timer ticks using the `pause` system call available in the recorded xv6 revision.
- **Lab 3:** A Bash script that analyses an access log and reports total requests and failures by user.
- **Lab 4:** An xv6 argument printer used to compare argument handling in Bash and the xv6 shell.
- **Lab 6:** An xv6 metadata viewer and recursive filename search, including an optional inode display.
- **Lab 7:** Linux C programs demonstrating `fork`, `execlp`, `waitpid`, zombies, output redirection, two-stage pipes and an optional three-stage pipeline. Separate variants demonstrate exec failure and a pipeline blocked by an open write end. [Experiment notes](lab7/report.md) explain the results.
- **Lab 8:** Separate xv6 sources for `run`, `pingpong` and the optional `primes` program, a Makefile with their `UPROGS` entries, Observe answers, and separate `ls` and failed-exec experiment variants. [Build instructions](lab8/README.md) explain how to use them in an existing xv6 tree.

The xv6 programs must be built inside an xv6 source tree: place their source files in `user/`, register the executables in the Makefile's `UPROGS` list, and run `make qemu`. The Lab 7 programs target Linux and can be compiled with GCC; they use a different user interface from the xv6 programs.
