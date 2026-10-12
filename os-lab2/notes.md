# Lab 2 · Meet the OS You Will Build

**Sergiu Gherasim, FAF-242** · Technical University of Moldova, FCIM · Operating Systems · Laboratory work no. 2

The formatted report is [`OS_Practical_Work_2_Report.docx`](OS_Practical_Work_2_Report.docx).  
The source code for the user program is [`sleep.c`](sleep.c).

## Purpose of the work

- Build the xv6 kernel with a RISC-V cross-compiler and boot it in QEMU.
- Use xv6 as a minimal Unix: run user commands, use pipes, and run the test suite.
- Inspect the source code of user programs and locate system call implementations in the kernel.
- Implement a custom xv6 user program (`sleep.c`), integrate it into the build system, and test it in xv6.

## Part 1. Build and Boot xv6

Cloning and building xv6 for RISC-V:

```bash
git clone https://github.com/mit-pdos/xv6-riscv.git
cd xv6-riscv
make qemu
```

The build compiles the kernel and user programs using the RISC-V toolchain, then launches QEMU to emulate the RISC-V hardware and boot the kernel:

```text
xv6 kernel is booting

hart 2 starting
hart 1 starting
init: starting sh
$ 
```

## Part 2. Using xv6

Running basic Unix utilities, pipes, and user tests inside xv6:

```text
$ cat README
$ echo hello xv6
$ ls | grep c
$ wc README
$ usertests -q
```

### Questions & Answers

**1. List three programs xv6 ships with.**

`cat`, `echo`, `wc`

**2. You just used a pipe ( | ) inside xv6. Which two OS features must exist for a pipe to work? (Hint: think processes and communication.)**

- Process management
- Inter-process communication

**3. In one sentence: how does the xv6 shell compare to the Linux shell from Lab 1?**

I have noticed that it is noticeably slower than the Linux shell and that it has a lower number of commands that I can use (for instance, `clear` is not present).

## Part 3. Source Code Exploration

Inspecting user programs and locating system calls in the kernel:

```bash
sed -n "1,40p" user/ls.c
sed -n "1,30p" user/cat.c
cat user/user.h
grep -n "sys_read\|sys_write" kernel/sysfile.c | head
```

### Questions & Answers

**1. Which system calls does `user/cat.c` use, and what does each one ask the kernel to do?**

It uses: `read`, `write`, `exit`, `open`, `close`.

- `read`: asks the kernel to read data from an open file descriptor into a buffer.
- `write`: asks the kernel to write data from a buffer to an open file descriptor (such as standard output).
- `exit`: asks the kernel to terminate the calling process.
- `open`: asks the kernel to open a specified file and return a file descriptor.
- `close`: asks the kernel to release/close an open file descriptor.

**2. In which kernel file, and on which line, is sys_read implemented?**

`sys_read` is implemented in `kernel/sysfile.c` on line 69.

```text
69:sys_read(void)
83:sys_write(void)
```

**3. In one sentence: what is the difference between the code in kernel/ and the code in user/?**

The code in `kernel/` is the operating system itself, the OS cannot run without this, while the code in `user/` is for programs that run on top of the kernel, the OS can run without this.

## Part 4. Adding a User Program (`sleep.c`)

Creating `user/sleep.c` to pause execution for a given number of ticks:

```c
#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if(argc != 2){
    fprintf(2, "usage: sleep <ticks>\n");
    exit(1);
  }
  pause(atoi(argv[1]));   // system call into the kernel
  exit(0);
}
```

Adding the program to `UPROGS` in the `Makefile`:

```make
UPROGS=\
	$U/_cat\
	$U/_echo\
	$U/_forktest\
	$U/_grep\
	$U/_init\
	$U/_kill\
	$U/_ln\
	$U/_ls\
	$U/_mkdir\
	$U/_rm\
	$U/_sh\
	$U/_stressfs\
	$U/_usertests\
	$U/_grind\
	$U/_wc\
	$U/_zombie\
	$U/_sleep\
```

Building and testing inside xv6:

```text
$ make qemu
...
$ sleep 10
$ sleep
usage: sleep <ticks>
```

When invoked with a tick argument (e.g. `sleep 10`), the process pauses before returning to the prompt. If invoked without an argument, it outputs the usage message.

## Conclusion

During this practical work, I explored the xv6 operating system and learned more about the interaction between user programs and the kernel. I worked with basic xv6 programs and observed how processes can communicate using pipes.

I also analyzed the cat program and identified the system calls it uses, such as read, write, open, close, and exit. By locating their implementations in the xv6 kernel, I gained a better understanding of how user programs request services from the operating system. Overall, this practical work helped me better understand the role of the kernel, system calls, and inter-process communication in an operating system.
