# Lab 7 Linux Processes Redirection and Pipes

Cimendur Rustem, FAF-243. Ubuntu Linux in VirtualBox. Experiments performed on 24 October 2026.

Working directory: `/home/vboxuser/os-lab7/run-20261024-210030`. Kernel `7.0.0-38-generic`; GCC `15.2.0`.

## Part 1 Fork exec and wait

```bash
gcc -Wall -Wextra -o run run.c && ./run
gcc -Wall -Wextra -o run_date run_date.c && ./run_date
gcc -Wall -Wextra -o run_bad run_bad.c && ./run_bad
```

### Observe 1 Why perror runs only after a failed exec

A successful `execlp` replaces the child process image and never returns to the next statement in this C program. With `nosuchprog`, the call failed and returned, so `perror("exec")` printed `exec: No such file or directory`. The child then executed the explicit `exit(127)`.

### Observe 2 Which PID the child reports as its parent

The child prints the value returned by `getppid()`, which identifies the process that forked it. In the original run, child 5405 printed 5404, matching the parent message. The date and failure variants likewise showed 5431 and 5447 as their respective parent PIDs.

### Observe 3 Statuses seen for ls and the missing command

The parent collected status 0 for ls and status 127 for nosuchprog. In the failure run, parent 5447 collected 127 from child 5449. This value came from the child error branch, rather than from an automatically assigned exec return status. The supplied parent still returns 0 from main after printing the child result.

## Part 2 A zombie on purpose

```bash
gcc -Wall -Wextra -o zombie zombie.c
./zombie > /dev/null &
sleep 2
ps -o pid,ppid,stat,cmd
sleep 20
ps -o pid,ppid,stat,cmd
```

### Observe 1 Meaning of the STAT letter

The ls child, PID 5478 with parent PID 5475, had STAT `Z` and appeared as `[ls] <defunct>`. Z means zombie: the child has terminated, but its parent has not yet collected its termination status. The parent itself had STAT S because it was sleeping.

### Observe 2 Which line makes the zombie disappear

After the pause, the defunct child line was gone. The call `waitpid(pid, &status, 0)` at line 25 of zombie.c collected the child status and reaped it. The shell wait command separately waited for the background parent to finish; it was the waitpid inside that parent that reaped ls.

## Part 3 Redirection

```bash
gcc -Wall -Wextra -o redirect redirect.c && ./redirect
head -3 out.txt
./redirect > parent_stdout.txt 2> parent_stderr.txt
cat parent_stdout.txt
cat parent_stderr.txt
```

### Observe 1 Why open returns descriptor one

The program printed `open gave descriptor 1`. Descriptor 1 had just been closed, while descriptors 0 and 2 remained open. Open selects the lowest unused descriptor, so the new file occupied slot 1.

### Observe 2 Why the unchanged ls writes to the file

Ls writes its listing to standard output, descriptor 1. The child had connected that descriptor to out.txt before exec, and ls inherited the connection. The screenshot showed the listing in the file, while the descriptor diagnostic stayed on stderr and the parent message stayed on its own stdout.

### Observe 3 Which descriptor redirects errors

To redirect standard error in the same way as `2> err.txt`, close descriptor 2 before opening err.txt. Assuming descriptors 0 and 1 remain open, the open call then reuses slot 2. The existing stdout connection can continue to display the normal listing.

## Part 4 A two-stage pipe

```bash
gcc -Wall -Wextra -o pipeline pipeline.c
./pipeline
ls /etc | wc -l
gcc -Wall -Wextra -o pipeline_broken pipeline_broken.c
./pipeline_broken
```

### Observe 1 Whether the two counts agree

Yes. Both `./pipeline` and `ls /etc | wc -l` printed 236. The working version was run again after the interruption in Figure 4.6 and again matched the shell with status 0. These are newline counts from ls output under a pipe; the value depends on the directory contents.

### Observe 2 Which child never finishes

The wc child remained blocked. The inspection showed PID 5600 running `wc -l` in `anon_pipe_read`, while its parent, PID 5598, waited in `do_wait`. The ls child had already finished and was absent from the displayed process tree.

### Observe 3 What wc is waiting for

Wc is waiting for EOF, which cannot arrive while the parent still holds an open descriptor for the pipe write end.

The broken pipeline was inspected from a second terminal: parent 5598 waited in do_wait; wc 5600 waited in anon_pipe_read. Parent fd 4 and wc fd 0 referenced pipe:[30118]. Ctrl+C stopped it with status 130. The working pipeline then printed 236 and exited with status 0.

## Optional three stages

```bash
gcc -Wall -Wextra -o pipeline3 pipeline3.c
./pipeline3
ls /usr/bin | grep x | wc -l
```

Both printed 229; the C program exited with status 0. It used two pipes and three children. Every process closed all four original pipe descriptors after its required duplications.

## Conclusion

Fork, exec and wait explain the process lifecycle. Redirecting descriptors changes where unchanged programs read and write. Closing unused pipe ends is necessary for readers to receive EOF and finish.

Detailed commands and screenshots are documented in `../lab7_report.docx`; figure numbers restart in each assignment part.
