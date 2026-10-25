# Lab 7 · Three Small Programs: Processes, Redirection, a Pipe

**Sergiu Gherasim, FAF-242** · Technical University of Moldova, FCIM · Operating Systems · Laboratory work no. 7

The formatted report is [`OS_Practical_Work_7_Report.tex`](OS_Practical_Work_7_Report.tex) (LaTeX, compiles with `LOGO_UTM.png` in the same folder).  
The source files for this lab are [`run.c`](run.c), [`zombie.c`](zombie.c), [`redirect.c`](redirect.c), [`pipeline.c`](pipeline.c) and the optional [`pipeline3.c`](pipeline3.c).

## Purpose of the work

- Start a program the way the shell does: `fork`, `exec`, `wait`
- Create a zombie on purpose and see when it disappears
- Implement `ls -l > out.txt` by rearranging file descriptors before `exec`
- Implement `ls /etc | wc -l` with one pipe and two children, and break it on purpose

```bash
mkdir -p ~/os-lab7 && cd ~/os-lab7
gcc --version | head -1     # gcc (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0
```

## Part 1. fork + exec + wait

```text
$ gcc -Wall -Wextra -o run run.c && ./run
parent: my PID is 4211
child:  my PID is 4212, my parent is 4211
total 76
-rwxrwxr-x 1 sergiu sergiu 70984 Oct 25 19:05 run
-rw-rw-r-- 1 sergiu sergiu  1047 Oct 25 19:04 run.c
parent: child 4212 ended with status 0
```

With `execlp("date", "date", (char *)NULL);`:

```text
parent: my PID is 4230
child:  my PID is 4231, my parent is 4230
Sun Oct 25 07:06:12 PM EET 2026
parent: child 4231 ended with status 0
```

With `execlp("nosuchprog", "nosuchprog", (char *)NULL);`:

```text
parent: my PID is 4248
child:  my PID is 4249, my parent is 4248
exec: No such file or directory
parent: child 4249 ended with status 127
```

### Questions & Answers

**1. `perror("exec")` is right after `execlp`. Why is it printed only with `nosuchprog`?**

A successful `exec` never returns: the child's program, including the lines after `execlp`, is replaced by `ls`/`date`. Only when `exec` fails (no `nosuchprog` in `PATH`) does it return −1, so the child continues to `perror` and `exit(127)`.

**2. Which number does the child print as "my parent", and whose PID is it?**

4211 — the PID of the `./run` process that called `fork` (the first line it printed).

**3. What status does the parent see with `ls`, and with `nosuchprog`?**

0 with `ls` (and `date`); 127 with `nosuchprog`, the value of `exit(127)` after the failed `exec`.

## Part 2. A Zombie, on Purpose

`zombie.c` = `run.c` + `sleep(20);` just before `waitpid`.

```text
$ ./zombie > /dev/null & sleep 2; ps -o pid,ppid,stat,cmd
[1] 4301
    PID    PPID STAT CMD
   3874    3866 Ss   -bash
   4301    3874 S    ./zombie
   4303    4301 Z    [ls] <defunct>
   4304    3874 R+   ps -o pid,ppid,stat,cmd
$ ps -o pid,ppid,stat,cmd          # 20 seconds later
    PID    PPID STAT CMD
   3874    3866 Ss   -bash
   4322    3874 R+   ps -o pid,ppid,stat,cmd
[1]+  Done                    ./zombie > /dev/null
```

### Questions & Answers

**1. Which letter does `ls` have in the STAT column, and what does it mean?**

`Z` — zombie (`<defunct>`): `ls` has finished, but its entry and exit status stay in the process table until the parent waits for it.

**2. After 20 seconds, is the `[ls] <defunct>` line still there? Which line removed it?**

No. `waitpid(pid, &status, 0);` collected the status, and the kernel removed the entry.

## Part 3. Redirection: ls -l > out.txt

```text
$ gcc -Wall -Wextra -o redirect redirect.c && ./redirect
open gave descriptor 1
done: the listing is in out.txt
$ head -3 out.txt
total 140
-rw-rw-r-- 1 sergiu sergiu     0 Oct 25 19:12 out.txt
-rwxrwxr-x 1 sergiu sergiu 71048 Oct 25 19:12 redirect
```

### Questions & Answers

**1. Which number did `open` give, and why exactly that one?**

1 — the kernel always returns the lowest free descriptor; 0 and 2 were open and 1 had just been closed.

**2. `ls` was not changed. Why does its listing not appear on the screen?**

`ls` always writes to descriptor 1. `exec` keeps the descriptor table, so the new `ls` inherited a descriptor 1 that points to `out.txt` instead of the terminal.

**3. To redirect the errors instead (`2> err.txt`), which descriptor would you close before `open`?**

Descriptor 2.

## Part 4. A Pipe: ls /etc | wc -l

```text
$ gcc -Wall -Wextra -o pipeline pipeline.c && ./pipeline
213
$ ls /etc | wc -l
213
```

After commenting out the parent's `close(p[1]);` the program hangs:

```text
$ ./pipeline
^C
# from a second terminal, while it hangs:
$ ps -o pid,ppid,stat,cmd -C pipeline,ls,wc
    PID    PPID STAT CMD
   4420    3874 S+   ./pipeline
   4422    4420 S+   wc -l
```

### Questions & Answers

**1. Did `./pipeline` and `ls /etc | wc -l` give the same number?**

Yes, both printed 213.

**2. After the change, which child never ends, `ls` or `wc`?**

`wc`. `ls` writes its output and exits; `wc` stays blocked in `read`.

**3. Why, in one sentence: what is `wc` waiting for?**

`wc` waits for end-of-file on the pipe, which comes only when all write ends are closed — and the parent still holds `p[1]` open.

## Optional: three stages

`pipeline3.c` builds `ls /usr/bin | grep x | wc -l` with two pipes and three children; every child and the parent close all four pipe ends after the `dup`s.

```text
$ gcc -Wall -Wextra -o pipeline3 pipeline3.c && ./pipeline3
187
$ ls /usr/bin | grep x | wc -l
187
```

## Conclusion

In this lab I reproduced the core of a shell in a few lines of C. `run.c` showed the three steps of starting a command — `fork`, `exec`, `wait` — and how the exit status travels back to the parent (0 on success, 127 when `exec` fails). `zombie.c` made visible the zombie state between a child's `exit` and the parent's `waitpid`.

`redirect.c` and `pipeline.c` showed that redirection and pipes need no help from the programs: the shell only rearranges descriptors between `fork` and `exec`, relying on "lowest free number". Breaking the pipeline taught me the key rule of pipes: every unused write end must be closed, otherwise the reader never gets end-of-file.
