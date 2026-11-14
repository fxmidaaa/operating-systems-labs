# Lab 10 Add Your Own System Call trace

Cimendur Rustem, FAF-243. Experiments performed on 14 November 2026.

## Purpose and environment

I extended the RISC-V version of xv6 with trace, a per-process system call
tracing facility. The source tree was `/home/vboxuser/xv6-riscv`, starting
from revision `06aad25`, in Ubuntu Linux inside VirtualBox. The tree already
contained earlier laboratory programs. README was 2441 bytes, syscall 22
was sync, and the kernel print interface was printk.

## Part 1 Number and user side

I added SYS_trace as 23 in kernel/syscall.h, declared int trace(int) in
user/user.h, added entry("trace") to user/usys.pl, created user/trace.c,
and registered _trace in Makefile UPROGS after _zombie.

The user program calls trace with the numeric mask and then execs the
requested command. The generated stub loads SYS_trace into a7, executes
ecall, and returns. The baseline boot printed LAB10_BASELINE_BOOT_OK.

```text
trace 32 grep hello README
3 trace: unknown sys call 23
trace: failed
```

This was the required intermediate checkpoint, before a kernel handler
existed. It confirmed that the request reached the kernel with number 23.

## Part 2 Kernel implementation

The trace_mask field was added to struct proc. sys_trace reads argument zero
with argint and stores it in myproc()->trace_mask. syscall.c declares and
registers sys_trace, maps syscall numbers to names, and prints selected
calls after storing their return value in trapframe->a0.

The fork implementation copies np->trace_mask = p->trace_mask so descendants
inherit the selection. allocproc initializes p->trace_mask = 0 so a reused
process slot cannot retain a previous process's mask. exec retains the
process and its trace setting.

After rebuilding, the required check succeeded:

```text
trace 32 grep hello README
3: syscall read -> 1023
3: syscall read -> 965
3: syscall read -> 453
3: syscall read -> 0
```

The successful reads sum to 2441 bytes, matching README. The final zero is EOF.

## Part 3 Mask experiments

```text
trace 2147483647 grep hello README
trace 2 usertests forkforkfork
trace 65536 echo hi
trace 2129920 grep hello README
echo LAB10_UNTRACED_OK
trace 0 echo LAB10_MASK_ZERO_OK
```

With mask 2147483647, PID 4 printed trace -> 0, exec -> 3, open -> 3,
the four read results, and close -> 0. The trace syscall itself appeared
because its handler enabled the mask before the dispatcher checked it.
The exec return value 3 is the argument count of the new command.
exit does not return to the post-call print hook.

The forkforkfork test produced fork records from multiple PIDs, including
5, 6, 7, 8 and 20 in the initial recording. Its end included fork -> -1
when resources were exhausted, followed by OK and ALL TESTS PASSED.
The negative fork results were expected by this test.

With mask 65536, PID 67 wrote two bytes for hi and one for the newline.
The first trace appeared immediately after hi; the second followed the
newline. With mask 2129920, PID 68 printed only open -> 3 and close -> 0.
The ordinary echo and zero-mask command printed their confirmation markers
without syscall trace records. Terminal input and output interleaved in the
last screenshot, but both markers were present.

## Observe answers

1. 32 is 1 << 5 and SYS_read is 5. Only the read bit is enabled; open and
   close have numbers 15 and 21, whose bits are zero.
2. np->trace_mask = p->trace_mask in the fork implementation copies the mask
   into each child, allowing different PIDs to print their own selected calls.
3. echo performs two writes for this invocation: hi and the newline. The
   kernel prints after each write returns, which explains the interleaving.
   Kernel printk output does not make another user write syscall.
4. The mask is (1 << 15) | (1 << 21) = 32768 + 2097152 = 2129920.

## Conclusion

The implementation connects the user declaration and generated ecall stub
to a numbered kernel handler. A process field controls tracing, fork copies
it, and the dispatcher prints the selected syscall names and return values.
Both the required read checkpoint and all requested mask experiments passed.

The screenshots and detailed explanations are in ../lab10_report.docx.
