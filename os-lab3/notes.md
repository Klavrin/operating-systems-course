# Lab 3 · The Command Line: Shell, Pipes and Text Filters

**Sergiu Gherasim, FAF-242** · Technical University of Moldova, FCIM · Operating Systems · Laboratory work no. 3

The formatted report is [`OS_Practical_Work_3_Report.docx`](OS_Practical_Work_3_Report.docx).  
The scripts for this lab are [`oneliners.sh`](oneliners.sh) and [`report.sh`](report.sh).

## Purpose of the work

- learn to move fast in bash: Tab completion, history search, finding help without a browser
- understand what the shell does to a command line before the program runs (globbing, quoting, expansions)
- control where output goes with redirection, chain commands with `;` `&&` `||`, run and stop background jobs
- answer real questions about a data file with one line of small filters joined by pipes
- turn those one-liners into a small script with argument checking, error handling and exit codes

## Part 1. Moving Fast in Bash

### Questions & Answers

**1. Which of cd, ls, echo are built-ins, and why MUST cd be one?**

`cd` and `echo` are built-ins, while `ls` is an external command. `cd` must be one because it needs to change the working directory in the current shell.

**2. How many directories are in your PATH, and in which one is ls?**

There are five directories in the `PATH` environment variable:

```text
/usr/local/bin
/usr/bin
/bin
/usr/local/games
/usr/games
```

`ls` is located in `/bin` (or `/usr/bin/ls`).

**3. What exit code does a command that is not found return?**

`127`

**4. Which option of ls sorts by modification time? (find it in man ls)**

`ls -t`

## Part 2. Globbing, Quoting, and Expansions

### Command Prediction & Observations

| Command | My prediction | Real output | Explanation (if different) |
|---|---|---|---|
| `echo *.c` | Print all .c files | All .c files | |
| `echo file?.txt` | Print all fileN.txt | Print all fileN.txt | |
| `echo [a-b]*` | Print all words that have a and b in it | `a.c alfa beta.txt b.c` | Prints all files starting with a or b |
| `echo *.xyz` | Print all .xyz files | `*.xyz` | Because there is no matching file |
| `ls alfa beta.txt` | Error? | Error | Bash treats them as two separate arguments |
| `ls "alfa beta.txt"` | Error? | `'alfa beta.txt'` | Displays the `alfa beta.txt` file |
| `echo '$HOME' "$HOME" \$HOME` | No idea honestly | `$HOME /home/sergiu $HOME` | Single quotes preserve literal text, double quotes expand variables |
| `echo {1..5} ; echo $((7 * 6))` | 1, 2, 3, 4, 5<br>42 | `1 2 3 4 5`<br>`42` | |
| `echo "today is $(date +%A), I am $(whoami)"` | It’s going to print the date and who I am | `today is Sunday, I am sergiu` | Command substitution `$()` executes commands inside double quotes |
| `touch log{1..3}.txt && ls log*` | Create log1.txt, log2.txt, log3.txt and then ls all the logs | `log1.txt log2.txt log3.txt` | Brace expansion generates file names, `&&` runs `ls` if `touch` succeeds |

### Questions & Answers

**1. echo knows nothing about files, yet echo *.c printed file names. Who produced them?**

The Bash shell produced the filenames through a process called globbing (pathname expansion).

**2. Why did ls alfa beta.txt fail? Give two different ways to fix it.**

Because bash interpreted the filename as two different arguments. One way to fix it is to escape the space with a backslash (`alfa\ beta.txt`) or wrap the filename in quotes (`"alfa beta.txt"`).

**3. What is the difference between single and double quotes?**

Single quotes preserve text literally, while double quotes preserve spaces but allow variable expansion and command substitution.

## Part 3. Redirection, Pipelines, and Background Jobs

### Questions & Answers

**1. Which file got the error message, and which got the listing?**

`err.txt` received the error message. `out.txt` received the directory listing of `/etc`.

**2. What is the difference between `> all.txt 2>&1` and `2>&1 > all.txt`? Try both and explain why the result differs.**

The order of redirection matters because Bash processes redirections from left to right. In the first case (`> all.txt 2>&1`), stdout is redirected to `all.txt`, and then stderr is redirected to where stdout currently points (the file). In the second case (`2>&1 > all.txt`), stderr is redirected to where stdout currently points (the terminal), and only stdout is redirected to the file.

**3. Why does `wc -l < access.log` print only a number, while `wc -l access.log` also prints the file name?**

The first command reads data from `stdin` and prints only the number of lines, while the second receives the filename as an argument and prints both the line count and the filename.

**4. Explain each operator in one sentence: `&&` and `||`.**

- `&&` (AND): Executes the second command only if the first command succeeds (exit code 0).
- `||` (OR): Executes the second command only if the first command fails (non-zero exit code).

### Background Job Commands

```bash
cd ~/os-lab3
clear
# Start a process in background
sleep 100 &
# List background jobs
jobs
# Terminate job number 1
kill %1
# Check job status
jobs
```

## Part 4. One-Liners over `access.log`

The one-liners implemented in [`oneliners.sh`](oneliners.sh):

```bash
# Q1. How many requests failed?
grep -c FAIL access.log

# Q2. How many different pages were requested, and which?
cut -d' ' -f5 access.log | sort -u | nl

# Q3. How many requests did each page get?
cut -d' ' -f5 access.log | sort | uniq -c

# Q4. Which user or users have the most failed requests, and how many?
grep FAIL access.log | cut -d' ' -f3 | sort | uniq -c | sort -rn | awk 'NR==1{max=$1} $1==max'

# Q5. How many requests did user3 make, and how many of them failed?
echo "user3: $(grep -cw user3 access.log) requests, $(grep -w user3 access.log | grep -c FAIL) failed"

# Q6. Print the last 3 failed requests, showing only time and user.
grep FAIL access.log | tail -3 | cut -d' ' -f2,3

# Q7. Which login shells appear in /etc/passwd, and how many accounts use each? (field 7, separator :)
cut -d: -f7 /etc/passwd | sort | uniq -c | sort -rn

# Q8. Why do ls /etc | wc -l and ls -l /etc | wc -l differ by exactly one?
echo "ls: $(ls /etc | wc -l) lines, ls -l: $(ls -l /etc | wc -l) lines, extra line: $(ls -l /etc | head -1)"

# Bonus. Lines where the minute ends in 0 and the request failed, and how many there are.
grep -E '^[0-9-]+ [0-9]{2}:[0-5]0 user[0-9]+ FAIL ' access.log
grep -cE '^[0-9-]+ [0-9]{2}:[0-5]0 user[0-9]+ FAIL ' access.log
```

## Part 5. Writing a Report Script

### Questions & Answers

**1. What does the first line `#!/bin/bash` do, and what happens if you run `./report.sh` before `chmod +x`?**

The shebang (`#!/bin/bash`) tells the system to execute the script using Bash. Without execute permission (`chmod +x`), running `./report.sh` results in a permission denied error.

**2. Why must you type `./report.sh` and not just `report.sh`?**

The `./` prefix specifies the current directory, which is not included in `PATH` by default. Without it, Bash searches only the directories in `PATH` and cannot find the script.

**3. Why does the script write the error with `>&2` and end with `exit 1`?**

`>&2` sends the error message to `stderr`, separating errors from normal output. `exit 1` terminates the script and signals that execution failed.

## Conclusion

During this practical work, I gained a better understanding of the Linux command line and Bash shell. I learned how to navigate the system, use command-line tools, work with wildcards, and understand the differences between shell built-ins and external commands. I also practiced input/output redirection, pipes, conditional operators, and background processes.

Additionally, I learned how to process log files using commands such as grep, cut, sort, uniq, and wc. Finally, I created a Bash script to automate log analysis, implemented basic error handling, and used cron to execute the script automatically.

Overall, this laboratory work helped me improve my command-line skills and understand how Bash can be used to automate tasks and efficiently manage a Linux system.
