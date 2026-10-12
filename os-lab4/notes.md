# Lab 4 · The xv6 Shell from the Outside

**Sergiu Gherasim, FAF-242** · Technical University of Moldova, FCIM · Operating Systems · Laboratory work no. 4

The formatted report is [`OS_Practical_Work_4_Report.pdf`](OS_Practical_Work_4_Report.pdf).  
The source files for this lab are [`args.c`](args.c), [`args_linux.c`](args_linux.c), and [`sleep.c`](sleep.c).

## Purpose of the work

- Compare the xv6 shell with modern Linux bash by running identical commands and contrasting feature sets
- Trace command-line argument processing and understand how globbing and quotes are handled before program execution
- Implement an argument-printing utility (`args.c` in xv6, `args_linux.c` in Linux) to observe `argc` and `argv` parsing
- Inspect the xv6 shell implementation (`user/sh.c`) to understand command types (EXEC, REDIR, PIPE, LIST, BACK) and built-ins (`cd`)

## Part 1. The Sleep System Call

In this part we need to rename the sleep system call to pause. However, since I already did it in the second lab, I kept the `pause` system call implementation and verified that it works.

As you might have noticed, I didn't rename the sleep program in `/user`. I instead replaced the `sleep` system call with the `pause` system call:

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

Testing the program inside xv6:

```text
$ sleep 10
$ sleep
usage: sleep <ticks>
```

## Part 2. Comparing xv6 Shell with Bash

### Command Comparison Table

| Command | Tests | bash | xv6 |
|---|---|---|---|
| `echo hello > f ; cat < f` | redirection | Works | Works |
| `ls \| grep READ` | pipe | Works – echos README | Works – but echos differently (prints name, type, inode, size) |
| `(echo a; echo b) \| wc` | grouping, list | Echos numbers | Echos "syntax". xv6 doesn't support multiline command entry like bash |
| `echo one ; echo two` | sequence | Echos "one \n two" | Echos "one \n two" |
| `echo bg &` | background | `[1] 4877` | `bg` |
| `ls *` | globbing | `f README`<br>`d:` | `ls: cannot open *` |
| `echo $HOME` | variables | `/home/sergiu` | `$HOME` |
| `echo "a b"` | quotes | `a b` | `"a b"` (xv6 doesn't recognize double quotes) |
| `mkdir d ; cd d ; ls ..` | PATH | `d f README` | `exec mkdir failed`<br>`exec cd failed`<br>Lists root directory entries |
| Arrow up, Tab, Ctrl+R | history, completion | Work | Don't work |

### Questions & Answers

**1. List the features both shells have, and the ones only bash has.**

**Both have:**
- Command execution
- Pipes (`|`)
- Input/output redirection (`<`, `>`)
- Background execution (`&`)
- Command grouping (`()`)
- Sequential execution (`;`)
- Directory navigation (`cd`)

**Only bash has:**
- Command history
- Tab completion
- Environment variables
- PATH lookup
- Job control (`jobs`, `fg`, `bg`)
- Advanced scripting (variables, loops, etc.)
- Command substitution (`$()`)
- Advanced command grouping

**2. In xv6, ls stops working after `cd d`. Why? Find a way to run ls from inside `d` anyway. (Hint: where do the xv6 programs live?)**

`ls` doesn't work inside `d` because we are not in the root directory and xv6 has no `PATH` variable to resolve binaries in other directories. We can run it from inside `d` by specifying the absolute path: `/ls`.

**3. Which missing feature would you miss most in daily work, and which would be easiest to add?**

I would probably miss command history and tab completions as I use them the most. The easiest to add would be command history, as it could just be a file that lists all of the commands entered in the current session.

## Part 3. Command-Line Arguments and Expansion

### Source Implementations

**xv6 (`args.c`):**

```c
#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  printf("argc = %d\n", argc);
  for (int i = 0; i < argc; i++)
    printf("argv[%d] = [%s]\n", i, argv[i]);
  exit(0);
}
```

**Linux (`args_linux.c`):**

```c
#include <stdio.h>

int main(int argc, char *argv[]) {
  printf("argc = %d\n", argc);
  for (int i = 0; i < argc; i++)
    printf("argv[%d] = [%s]\n", i, argv[i]);
  return 0;
}
```

### Questions & Answers

**1. Paste both outputs for `args *.c "x y"`. How many arguments did each program receive?**

**Bash:**
```text
argc = 3
argv[0] = [./args]
argv[1] = [args_linux.c]
argv[2] = [x y]
```

**xv6:**
```text
argc = 4
argv[0] = [args]
argv[1] = [*.c]
argv[2] = ["x]
argv[3] = [y"]
```

Bash receives 3 arguments, while xv6 receives 4.

**2. So who turns `*.c` into file names and removes the quotes: the program, the kernel, or the shell?**

The shell is responsible for expanding `*.c` into matching filenames and removing quotes before passing arguments to the program. In bash, wildcard expansion and quote removal happen automatically. In xv6, the shell does not support these features, so the wildcard and quotes are passed as literal characters.

**3. What happened to the extra spaces in `one two three`?**

The shell treats consecutive unquoted spaces as argument separators and discards the extra spaces. Therefore, `one two three` is passed as three separate arguments, regardless of how many spaces are between the words. Both bash and xv6 behave similarly in this case.

## Part 4. The xv6 Shell Internals (`user/sh.c`)

### Questions & Answers

**1. Name the five kinds of command the xv6 shell knows and the character that produces each.**

- `EXEC` (normal commands)
- `REDIR` (`<`, `>`, `>>`)
- `PIPE` (`|`)
- `LIST` (`;`)
- `BACK` (`&`)

Defined in `user/sh.c`:

```c
#define EXEC  1
#define REDIR 2
#define PIPE  3
#define LIST  4
#define BACK  5
#define MAXARGS 10
```

**2. Where is cd handled, and why is it not run as a separate program?**

`cd` is handled directly in `user/sh.c` as a built-in command:

```c
if(cmd[0] == 'c' && cmd[1] == 'd' && cmd[2] == ' '){
  // Chdir must be called by the parent, not the child.
  cmd[strlen(cmd)-1] = 0;  // chop \n
  if(chdir(cmd+3) < 0)
    fprintf(2, "cannot cd %s\n", cmd+3);
  continue;
}
```

It must run in the shell process itself because a separate program would run as a child process and only change its own working directory before terminating, leaving the parent shell's working directory unchanged.

**3. Search sh.c for any code that handles `*` or `$`. What does that explain about Part 2?**

Wildcard (`*`) and variable (`$`) expansion are not implemented in `sh.c`. This explains why xv6 treats them as literal characters instead of expanding them like bash does.

## Conclusion

In this practical work, I explored the functionality of the xv6 shell and compared it with the Linux bash shell. I learned how both shells handle command execution, pipes, redirection, background processes, and directory navigation. By testing the same commands in both environments, I observed that xv6 provides basic shell functionality but lacks advanced features such as command history, tab completion, wildcard expansion, and environment variables.

I also created a C program to examine how command-line arguments are processed and discovered that the shell, rather than the kernel or the program itself, is responsible for interpreting wildcards and quotes. Finally, by examining the source code of `sh.c`, I gained a better understanding of how the xv6 shell parses and executes commands and why `cd` must be implemented as a built-in command.

Overall, this laboratory helped me better understand how shells work internally and appreciate the additional functionality provided by modern shells such as bash.
