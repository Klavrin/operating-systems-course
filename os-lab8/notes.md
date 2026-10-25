# Lab 8 · The Same Calls Inside Your Own OS

**Sergiu Gherasim, FAF-242** · Technical University of Moldova, FCIM · Operating Systems · Laboratory work no. 8

The formatted report is [`OS_Practical_Work_8_Report.tex`](OS_Practical_Work_8_Report.tex) (LaTeX, compiles with `LOGO_UTM.png` in the same folder).  
The source files for this lab are [`run.c`](run.c), [`pingpong.c`](pingpong.c), the optional [`primes.c`](primes.c) and the [`Makefile`](Makefile) with the new `UPROGS` lines.

## Purpose of the work

- Run the `fork` / `exec` / `wait` program from Lab 7 inside xv6 and compare the calls
- Make two processes talk in both directions through two pipes (`pingpong`)
- Optionally, build a chain of processes connected by pipes that prints primes (`primes`)

```make
UPROGS=\
	$U/_cat\
	...
	$U/_sync\
	$U/_run\
	$U/_pingpong\
	$U/_primes\
```

## Part 1. run, inside xv6

```text
$ make qemu
...
init: starting sh
$ run
parent: my pid is 3
child: my pid is 4
hello from the child
parent: child 4 ended with status 0
```

With `char *args[] = { "ls", 0 }; exec("ls", args);`:

```text
$ run
parent: my pid is 3
child: my pid is 4
.              1 1 1024
..             1 1 1024
README         2 2 2441
cat            2 3 35376
...
run            2 25 34560
pingpong       2 26 34384
primes         2 27 34720
console        3 28 0
parent: child 4 ended with status 0
```

With `exec("nosuchprog", args);`:

```text
$ run
parent: my pid is 3
child: my pid is 4
exec failed
parent: child 4 ended with status 1
```

### Questions & Answers

**1. Write the three calls that `run.c` uses in xv6 and the three calls it used on Linux in Lab 7. What is the same?**

| Step | xv6 | Linux (Lab 7) |
|---|---|---|
| copy | `fork()` | `fork()` |
| become | `exec("echo", args)` | `execlp("ls", "ls", "-l", NULL)` |
| wait | `wait(&status)` | `waitpid(pid, &status, 0)` |

The model is the same: `fork` returns twice (0 in the child), `exec` replaces the program and returns only on failure, and `wait` blocks the parent until the child exits and returns its status. xv6 just has the bare calls: no `PATH` search and no `WEXITSTATUS` — `wait` gives the exit value directly.

**2. Who prints "hello from the child": the code of `run`, or `echo`?**

`echo`. After `exec` the child (PID 4) runs `user/echo.c`, which prints its arguments `hello from the child`.

**3. Why is the status 1 here and 127 on Linux?**

The status is just the number the child passes to `exit` after a failed `exec`: `exit(1)` in the xv6 `run.c`, `exit(127)` in the Linux one. The kernel only carries it from `exit` to `wait`.

## Part 2. pingpong

```c
  if(fork() == 0){
    close(p2c[1]); close(c2p[0]);       // the ends the child does not use
    read(p2c[0], &b, 1);                // blocks until the parent writes
    printf("%d: received ping\n", getpid());
    write(c2p[1], &b, 1);               // send the same byte back
    exit(0);
  }
  close(p2c[0]); close(c2p[1]);         // the ends the parent does not use
  write(p2c[1], &b, 1);                 // ping
  read(c2p[0], &b, 1);                  // blocks until the child replies
  printf("%d: received pong\n", getpid());
  wait(0);
```

(The full file in [`pingpong.c`](pingpong.c) also checks the return values of `read`.)

```text
$ pingpong
6: received ping
5: received pong
```

### Questions & Answers

**1. Why do you need TWO pipes, and not one for both directions?**

A pipe is one one-way buffer that any holder of the read end can read. With one pipe, the parent could read back its own ping before the child reads it, and the child would wait forever. Two pipes give each direction its own channel.

**2. Nothing in your code says "wait until the child has replied". Which call makes the parent wait?**

`read(c2p[0], &b, 1)` in the parent: the pipe is empty, so the kernel puts the parent to sleep until the child writes the pong. The child's `read(p2c[0], ...)` blocks in the same way until the ping arrives.

## Optional: primes

Each stage reads its first number `p` (a prime), prints it, creates a pipe and a child for the next stage and forwards only the numbers not divisible by `p`. Unused pipe ends are closed immediately.

```text
$ primes
prime 2
prime 3
prime 5
prime 7
prime 11
prime 13
prime 17
prime 19
prime 23
prime 29
prime 31
```

## Conclusion

In this lab I ran the Lab 7 program inside xv6 and confirmed that the process model is the same: `fork` copies, `exec` replaces the program, `wait` collects the status. Only the user library differs (no `PATH` search, no `WEXITSTATUS`), and the status after a failed `exec` is whatever the program passes to `exit` — 1 here, 127 on Linux.

With `pingpong` I made two processes communicate both ways and learned why each direction needs its own pipe, why unused ends must be closed, and that the blocking `read` is what synchronises the processes. The optional `primes` extended the same idea to a whole chain of processes.
