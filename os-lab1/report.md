# Lab 1 · First Contact with the Operating System

**Sergiu Gherasim, FAF-242** · Technical University of Moldova, FCIM · Operating Systems · Laboratory work no. 1

The formatted report is [`OS_Practical_Work_1_Report.docx`](OS_Practical_Work_1_Report.docx).

## Purpose of the work

- find out what machine and system I'm working on: user, kernel, architecture, uptime
- move around the file tree and read ls -l output (type, permissions, links, owner, size)
- create, copy, rename and delete files and directories, and change their permissions with chmod
- see processes from the outside (ps, top), run a job in the background and stop it with kill
- look at a single process, memory, disks and devices through the files the kernel exposes in /proc and /dev

---

```
❯ whoami
sergiugherasim
```

---

```
❯ uname -a
Darwin Sergius-MacBook-Pro.local 23.6.0 Darwin Kernel Version 23.6.0: Tue Jul 21 21:57:26 PDT 2026; root:xnu-10063.141.1.713.39~1/RELEASE_ARM64_T6000 arm64
```

---

```
❯ uptime
16:42  up 5 days, 14:57, 2 users, load averages: 2.89 2.27 2.24
```

---

### Part 1

1.

```
❯ pwd
/Users/sergiugherasim/Developer/Learning/operating-systems-lab
```

---

```
❯ ls -la /
total 10
drwxr-xr-x  22 root  wheel   704 Jul 22 13:19 .
drwxr-xr-x  22 root  wheel   704 Jul 22 13:19 ..
lrwxr-xr-x   1 root  admin    36 Jul 22 13:19 .VolumeIcon.icns -> System/Volumes/Data/.VolumeIcon.icns
----------   1 root  admin     0 Jul 22 13:19 .file
drwxr-xr-x   2 root  wheel    64 Jul 22 13:19 .nofollow
drwxr-xr-x   2 root  wheel    64 Jul 22 13:19 .resolve
drwxr-xr-x   2 root  wheel    64 Jul 22 13:19 .vol
drwxrwxr-x  65 root  admin  2080 Sep 26 16:39 Applications
drwxr-xr-x  70 root  wheel  2240 Sep 17 12:13 Library
drwxr-xr-x@ 10 root  wheel   320 Jul 22 13:19 System
drwxr-xr-x   5 root  admin   160 Aug  3 01:31 Users
drwxr-xr-x   3 root  wheel    96 Sep 23 01:46 Volumes
drwxr-xr-x@ 39 root  wheel  1248 Jul 22 13:19 bin
drwxr-xr-x   2 root  wheel    64 Jul 14  2022 cores
dr-xr-xr-x   4 root  wheel  4984 Sep 23 01:46 dev
lrwxr-xr-x@  1 root  wheel    11 Jul 22 13:19 etc -> private/etc
lrwxr-xr-x   1 root  wheel    25 Sep 23 01:46 home -> /System/Volumes/Data/home
drwxr-xr-x   4 root  wheel   128 Feb 28  2025 opt
drwxr-xr-x   6 root  wheel   192 Sep 23 01:46 private
drwxr-xr-x@ 64 root  wheel  2048 Jul 22 13:19 sbin
lrwxr-xr-x@  1 root  wheel    11 Jul 22 13:19 tmp -> private/tmp
drwxr-xr-x@ 11 root  wheel   352 Jul 22 13:19 usr
lrwxr-xr-x@  1 root  wheel    11 Jul 22 13:19 var -> private/var
```

---

```
❯ ls -la ~
total 2248
drwxr-x---+ 106 sergiugherasim  staff    3392 Sep 28 16:45 .
drwxr-xr-x    5 root            admin     160 Aug  3 01:31 ..
-r--------    1 sergiugherasim  staff       7 Aug  4 22:58 .CFUserTextEncoding
-rw-r--r--@   1 sergiugherasim  staff   28676 Sep 28 16:30 .DS_Store
drwx------+  82 sergiugherasim  staff    2624 Sep 28 16:38 .Trash
-rw-------    1 sergiugherasim  staff      49 Apr 29  2025 .Xauthority
drwxr-xr-x@   4 sergiugherasim  staff     128 Sep 21 12:20 .agents
drwxr-xr-x@   3 sergiugherasim  staff      96 Sep 21 12:21 .aider-desk
drwxr-xr-x@   5 sergiugherasim  staff     160 Jan 30  2026 .antigravity
drwxr-xr-x    5 sergiugherasim  staff     160 Jul 31  2025 .aspnet
drwxr-xr-x@   4 sergiugherasim  staff     128 Jul  8  2025 .azuredatastudio
-rw-------    1 sergiugherasim  staff      11 Jun 15 12:04 .bash_history
drwxr-xr-x    5 sergiugherasim  staff     160 Mar  7  2026 .bun
drwxr-xr-x   28 sergiugherasim  staff     896 Sep 28 16:41 .cache
drwxr-xr-x   27 sergiugherasim  staff     864 Sep 25 14:29 .claude
drwxr-xr-x   18 sergiugherasim  staff     576 Sep 25 14:31 .claude-mem
-rw-------@   1 sergiugherasim  staff  111312 Sep 25 15:12 .claude.json
drwxr-xr-x    3 sergiugherasim  staff      96 May 10  2025 .cmake
drwxr-xr-x    3 sergiugherasim  staff      96 Sep  7  2025 .cocoapods
drwxr-xr-x   21 sergiugherasim  staff     672 Jul 31  2025 .codeium
drwxr-xr-x@  56 sergiugherasim  staff    1792 Sep 26 14:08 .codex
drwxr-xr-x   14 sergiugherasim  staff     448 Aug  9 19:47 .config
drwxr-xr-x   10 sergiugherasim  staff     320 Jul 31  2025 .console-ninja
drwxr-xr-x   12 sergiugherasim  staff     384 Jun 11 17:50 .copilot
drwx------    3 sergiugherasim  staff      96 Dec  3  2023 .cups
drwxr-xr-x@   6 sergiugherasim  staff     192 Nov  4  2025 .cursor
drwxr-xr-x@   5 sergiugherasim  staff     160 Jul  9 16:08 .devin
drwxr-xr-x@  16 sergiugherasim  staff     512 Jul  9 21:51 .docker
drwxr-xr-x   16 sergiugherasim  staff     512 Jul 31  2025 .dotnet
drwxr-xr-x   12 sergiugherasim  staff     384 Sep  9 15:48 .expo
drwxr-xr-x    3 sergiugherasim  staff      96 Mar  1  2025 .foxrc
drwxr-xr-x    4 sergiugherasim  staff     128 Aug  3 12:22 .gem
drwxr-xr-x   17 sergiugherasim  staff     544 Sep 26 16:36 .gemini
drwxr-xr-x   20 sergiugherasim  staff     640 Sep 28 16:41 .git
-rw-------    1 sergiugherasim  staff     173 Sep  8 15:46 .git-credentials
-rw-r--r--    1 sergiugherasim  staff     267 Sep  8 15:38 .gitconfig
drwxr-xr-x    5 sergiugherasim  staff     160 Aug 10 18:25 .gk
-rw-r--r--@   1 sergiugherasim  staff    6975 Apr 21  2024 .hyper.js
drwxr-xr-x@   7 sergiugherasim  staff     224 Jul 31  2025 .hyper_plugins
-rw-r--r--    1 sergiugherasim  staff     304 Sep  1  2025 .ideavimrc
drwxr-xr-x    5 sergiugherasim  staff     160 Sep  7  2025 .ionic
drwxr-xr-x    4 sergiugherasim  staff     128 Jul 31  2025 .ipython
drwxr-xr-x@   4 sergiugherasim  staff     128 Nov 14  2024 .jupyter
-rw-------    1 sergiugherasim  staff      20 Sep  8 15:30 .lesshst
drwxr-xr-x    6 sergiugherasim  staff     192 Jul 31  2025 .local
drwxr-xr-x    3 sergiugherasim  staff      96 Oct 29  2025 .m2
drwxr-xr-x@   3 sergiugherasim  staff      96 Nov 14  2024 .matplotlib
drwx------    5 sergiugherasim  staff     160 Jul 31  2025 .mongodb
drwxr-xr-x    3 sergiugherasim  staff      96 May 21  2023 .mono
-rw-------    1 sergiugherasim  staff     386 Oct  4  2023 .node_repl_history
drwxr-xr-x    9 sergiugherasim  staff     288 Apr 28 18:11 .npm
-rw-------    1 sergiugherasim  staff      74 Aug  9  2023 .npmrc
drwxr-xr-x    5 sergiugherasim  staff     160 Jul 31  2025 .nuget
drwxr-xr-x   32 sergiugherasim  staff    1024 Aug 10 01:40 .nvm
drwxr-xr-x   23 sergiugherasim  staff     736 Apr  6 20:58 .oh-my-zsh
drwxr-xr-x    8 sergiugherasim  staff     256 Aug 10 02:01 .omniroute
drwxr-xr-x@   3 sergiugherasim  staff      96 Aug 16 20:56 .openjfx
drwxr-xr-x   11 sergiugherasim  staff     352 Jul 31  2025 .orbstack
-rw-r--r--    1 sergiugherasim  staff   87597 Apr 29  2023 .p10k.zsh
-rw-r--r--@   1 sergiugherasim  staff     258 May 14 14:20 .packettracer
drwx------   11 sergiugherasim  staff     352 Aug  4 00:01 .parsec
drwx------@  10 sergiugherasim  staff     320 Jul 31  2025 .pgadmin
drwxr-xr-x@   4 sergiugherasim  staff     128 Feb 25  2025 .plastic4
drwxr-xr-x    3 sergiugherasim  staff      96 Oct 24  2025 .pnpm-state
-rw-------    1 sergiugherasim  staff    3126 Apr 15 13:14 .psql_history
drwxr-xr-x@  15 sergiugherasim  staff     480 Sep  2 13:00 .qoder
drwxr-xr-x@   8 sergiugherasim  staff     256 Aug  7 16:39 .qodersec
drwxr-xr-x    7 sergiugherasim  staff     224 Jul 31  2025 .rest-client
drwx------    7 sergiugherasim  staff     224 Sep 28 12:54 .ssh
drwxr-xr-x    5 sergiugherasim  staff     160 Aug  7 17:26 .swiftpm
drwxr-xr-x    7 sergiugherasim  staff     224 Sep  3  2025 .templateengine
drwxr-xr-x    5 sergiugherasim  staff     160 Jul 31  2025 .tmux
-rw-r--r--    1 sergiugherasim  staff     682 Jan 17  2025 .tmux.conf
drwxr-xr-x@   4 sergiugherasim  staff     128 Jan 24  2025 .trae
-rw-------@   1 sergiugherasim  staff   24045 Aug  7 17:20 .viminfo
drwxr-xr-x    6 sergiugherasim  staff     192 Jul 31  2025 .vscode
drwxr-xr-x    9 sergiugherasim  staff     288 Sep 27  2025 .wine
-rw-r--r--    1 sergiugherasim  staff     116 Jul 21 18:21 .yarnrc
-rw-r--r--    1 sergiugherasim  staff   50333 Sep 28 16:41 .zcompdump-Sergius-MacBook-Pro-5.9
-rw-r--r--    1 sergiugherasim  staff   37659 Jul  1 12:14 .zcompdump-Sergius-MacBook-Pro-5.9.Sergius-MacBook-Pro.local.22636
-rw-r--r--    1 sergiugherasim  staff   37659 May 11 02:54 .zcompdump-Sergius-MacBook-Pro-5.9.Sergius-MacBook-Pro.local.94556
-r--r--r--    1 sergiugherasim  staff  117616 Sep 28 16:41 .zcompdump-Sergius-MacBook-Pro-5.9.zwc
-rw-r--r--    1 sergiugherasim  staff   48217 May  8  2023 .zcompdump-Sergiu’s MacBook Pro-5.8.1
-rw-r--r--    1 sergiugherasim  staff   50589 Oct  7  2025 .zcompdump-Sergiu’s MacBook Pro-5.9
-rw-r--r--    1 sergiugherasim  staff   45937 Jun 11  2025 .zcompdump-Sergiu’s MacBook Pro-5.9.Sergius-MacBook-Pro.local.1636
-rw-r--r--    1 sergiugherasim  staff   44822 Jun 11  2025 .zcompdump-Sergiu’s MacBook Pro-5.9.Sergius-MacBook-Pro.local.1638
-rw-r--r--    1 sergiugherasim  staff   44589 Jun 11  2025 .zcompdump-Sergiu’s MacBook Pro-5.9.Sergius-MacBook-Pro.local.1639
-rw-r--r--    1 sergiugherasim  staff     197 Jul 21  2025 .zprofile
-rw-------    1 sergiugherasim  staff  315293 Sep 28 16:45 .zsh_history
drwx------    4 sergiugherasim  staff     128 Sep 24 15:12 .zsh_sessions
-rw-r--r--@   1 sergiugherasim  staff    5229 Aug 10 01:39 .zshrc
-rw-r--r--@   1 sergiugherasim  staff     227 Sep 15 15:02 AGENTS.md
drwx------@   7 sergiugherasim  staff     224 Apr  6 21:05 Applications
drwxr-xr-x   12 sergiugherasim  staff     384 May 14 14:27 Cisco Packet Tracer 9.0.0
drwxr-xr-x@   7 sergiugherasim  staff     224 Dec  8  2025 DataGripProjects
drwx------@  18 sergiugherasim  staff     576 Sep 25 14:28 Desktop
drwxr-xr-x   20 sergiugherasim  staff     640 Sep  8 15:40 Developer
drwx------@  17 sergiugherasim  staff     544 Sep 15 16:09 Documents
drwx------@ 333 sergiugherasim  staff   10656 Sep 28 16:40 Downloads
drwx------@ 113 sergiugherasim  staff    3616 Aug 17 00:28 Library
drwxr-xr-x@   4 sergiugherasim  staff     128 May  4 21:27 Local Sites
drwx------    8 sergiugherasim  staff     256 Sep 21  2025 Movies
drwx------+   7 sergiugherasim  staff     224 Oct  1  2023 Music
drwx------+   6 sergiugherasim  staff     192 Oct  1  2023 Pictures
drwxr-xr-x+   5 sergiugherasim  staff     160 Mar 19  2024 Public
drwxr-xr-x    9 sergiugherasim  staff     288 Aug 10 01:08 fibonacci-venv
```

---

```
❯ cd /etc && ls | head
afpovertcp.cfg
aliases
aliases.db
apache2
asl
asl.conf
auto_home
auto_master
autofs.conf
bashrc

# I am now in /etc
   /etc
```

---

2.

```
    ~/Developer/Learning/operating-systems-lab/os-lab1 
❯ mkdir demo && cd demo # this creates a demo directory and then changes the directory to demo
    ~/Developer/Learning/operating-systems-lab/os-lab1/demo 
❯ echo "hello operating systems" > note.txt # now we are creating a note.txt file and writing to it the sentence
    ~/Developer/Learning/operating-systems-lab/os-lab1/demo 
❯ cp note.txt copy.txt # this copies `note.txt` and gives it a new name
    ~/Developer/Learning/operating-systems-lab/os-lab1/demo 
❯ mv copy.txt renamed.txt # this is something i find very interesting. There is no dedicated command for renaming files in linux. What you do is you copy the existing file, give it a new name, and then the old name gets removed.
    ~/Developer/Learning/operating-systems-lab/os-lab1/demo 
❯ ls -l # this lists the files inside the current dir in a list format. I often like to use `ll` for this. `ll` and `ls -l` are not exactly the same, but oftentimes these are used interchangeably
total 16
-rw-r--r--  1 sergiugherasim  staff  24 Sep 28 16:47 note.txt
-rw-r--r--  1 sergiugherasim  staff  24 Sep 28 16:47 renamed.txt
    ~/Developer/Learning/operating-systems-lab/os-lab1/demo 
❯ rm renamed.txt # rm is "remove"
    ~/Developer/Learning/operating-systems-lab/os-lab1/demo 
❯ # no message outputed
```

3.

```
❯ ls -l note.txt
-rw-r--r--  1 sergiugherasim  staff  24 Sep 28 16:47 note.txt

# -rw-r--r-- --> the first `rw` is for _owners_, the second `r` is for the _group_ and then the last `r` is for the _others_
```

---

```
❯ chmod 600 note.txt
# nothing written to the terminal,
```

---

```
❯ ls -l note.txt
-rw-------  1 sergiugherasim  staff  24 Sep 28 16:47 note.txt
# only the _owner_ has `rw`, the rest have `-` which means then don't have any permissions
```

---

```
chmod 644 note.txt
# change mode 644, means 6 - read + write, 4 - read, and 4 read again. So now the owner can read and write, the group and read and others can also read

1. I own the files "sergiugherasim"
2. The ten chars at the start show the permissions of the owner, group and others.
3.
❯ ll /
total 10
drwxrwxr-x  65 root  admin   2.0K Sep 26 16:39 Applications
drwxr-xr-x  70 root  wheel   2.2K Sep 17 12:13 Library
drwxr-xr-x@ 10 root  wheel   320B Jul 22 13:19 System
drwxr-xr-x   5 root  admin   160B Aug  3 01:31 Users
drwxr-xr-x   3 root  wheel    96B Sep 23 01:46 Volumes
drwxr-xr-x@ 39 root  wheel   1.2K Jul 22 13:19 bin
drwxr-xr-x   2 root  wheel    64B Jul 14  2022 cores
dr-xr-xr-x   4 root  wheel   4.8K Sep 23 01:46 dev
lrwxr-xr-x@  1 root  wheel    11B Jul 22 13:19 etc -> private/etc
lrwxr-xr-x   1 root  wheel    25B Sep 23 01:46 home -> /System/Volumes/Data/home
drwxr-xr-x   4 root  wheel   128B Feb 28  2025 opt
drwxr-xr-x   6 root  wheel   192B Sep 23 01:46 private
drwxr-xr-x@ 64 root  wheel   2.0K Jul 22 13:19 sbin
lrwxr-xr-x@  1 root  wheel    11B Jul 22 13:19 tmp -> private/tmp
drwxr-xr-x@ 11 root  wheel   352B Jul 22 13:19 usr
lrwxr-xr-x@  1 root  wheel    11B Jul 22 13:19 var -> private/var

/home -> the home directory with my personal files
/etc -> the system wide configuration
```

### Part 2

```
❯ ps aux | head
USER               PID  %CPU %MEM      VSZ    RSS   TT  STAT STARTED      TIME COMMAND
_windowserver      172  17.4  1.0 414182000 174560   ??  Ss   Wed01AM 519:08.42 /System/Library/PrivateFrameworks/SkyLight.framework/Resources/WindowServer -daemon
sergiugherasim   50466  11.2  1.5 430650752 246160   ??  R     5:03PM   1:36.36 /Applications/Zed.app/Contents/MacOS/zed
sergiugherasim     434  10.9  0.9 416822576 149440   ??  S    Wed01AM   6:32.80 /System/Library/CoreServices/Finder.app/Contents/MacOS/Finder
sergiugherasim   39895   8.1  3.2 412987408 541536   ??  S    Sat07PM  40:55.87 /Applications/Telegram.app/Contents/MacOS/Telegram
sergiugherasim   51403   5.5  0.1 411149824  10304 s001  S     5:14PM   0:00.49 -zsh
sergiugherasim   46084   4.9  1.3 461746112 225696   ??  S    12:54PM   4:16.72 /Applications/Brave Browser.app/Contents/Frameworks/Brave Browser Framework.framework/Versions/154.1.96.59/Helpers/Brave Browser Helper.app/Contents/MacOS/Brave Browser Helper --type=gpu-process --gpu-recent-crash-count=0 --metrics-client-id=2cb9ee0a-382a-4b3c-b766-3a943b4d8b96 --start-stack-profiler --gpu-preferences=WAAAAAAAAAAgAQAEAAAAAAAAAAAAAGAAQAAAAAAAAAADAAAAAAAAADgAAAAAAAAAAAAAAAAAAAACAAAAAAAAAAAAAAAAAAAAKAAAAAAAAAAoAAAAAAAAAAgAAAAAAAAADAAAAAEAAAAAAAAAAAAAAAgAAAAAAAAACAAAAAAAAAA= --shared-files --metrics-shmem-handle=1752395122,r,971570119388850816,1760235314165666190,262144 --field-trial-handle=1718379636,r,9290950812348410734,12239048362880553992,262144 --variations-seed-version=main@ac7b120e60fb2bfa25cdf1c026ec26785f370dc6 --pseudonymization-salt-handle=1935764596,r,2385777802457045313,12038131507582500958,4 --trace-process-track-uuid=3190708988185955192 --seatbelt-client=33
sergiugherasim   46608   1.6  3.4 1928137904 571696   ??  S     4:31PM   1:29.38 /Applications/Brave Browser.app/Contents/Frameworks/Brave Browser Framework.framework/Versions/154.1.96.59/Helpers/Brave Browser Helper (Renderer).app/Contents/MacOS/Brave Browser Helper (Renderer) --type=renderer --metrics-client-id=2cb9ee0a-382a-4b3c-b766-3a943b4d8b96 --enable-distillability-service --origin-trial-public-key=bYUKPJoPnCxeNvu72j4EmPuK7tr1PAC7SHh8ld9Mw3E=,fMS4mpO6buLQ/QMd+zJmxzty/VQ6B1EUZqoCU04zoRU= --lang=en-GB --num-raster-threads=4 --enable-zero-copy --enable-gpu-memory-buffer-compositor-resources --enable-main-frame-before-activation --renderer-client-id=143 --launch-time-ticks=129372804630 --shared-files --metrics-shmem-handle=1752395122,r,6937944381607965835,12080927541858317718,1572864 --field-trial-handle=1718379636,r,9290950812348410734,12239048362880553992,262144 --variations-seed-version=main@ac7b120e60fb2bfa25cdf1c026ec26785f370dc6 --pseudonymization-salt-handle=1935764596,r,2385777802457045313,12038131507582500958,4 --trace-process-track-uuid=3190709120308855901 --seatbelt-client=224
sergiugherasim   46607   1.4  1.2 1926150976 202656   ??  S     4:31PM   0:15.04 /Applications/Brave Browser.app/Contents/Frameworks/Brave Browser Framework.framework/Versions/154.1.96.59/Helpers/Brave Browser Helper (Renderer).app/Contents/MacOS/Brave Browser Helper (Renderer) --type=renderer --metrics-client-id=2cb9ee0a-382a-4b3c-b766-3a943b4d8b96 --enable-distillability-service --origin-trial-public-key=bYUKPJoPnCxeNvu72j4EmPuK7tr1PAC7SHh8ld9Mw3E=,fMS4mpO6buLQ/QMd+zJmxzty/VQ6B1EUZqoCU04zoRU= --lang=en-GB --num-raster-threads=4 --enable-zero-copy --enable-gpu-memory-buffer-compositor-resources --enable-main-frame-before-activation --renderer-client-id=142 --launch-time-ticks=129368659281 --shared-files --metrics-shmem-handle=1752395122,r,4547326353232927141,11072492106227834770,1572864 --field-trial-handle=1718379636,r,9290950812348410734,12239048362880553992,262144 --variations-seed-version=main@ac7b120e60fb2bfa25cdf1c026ec26785f370dc6 --pseudonymization-salt-handle=1935764596,r,2385777802457045313,12038131507582500958,4 --trace-process-track-uuid=3190709119371814052 --seatbelt-client=216
sergiugherasim   46078   1.1  4.3 461939856 719072   ??  S    12:54PM   5:43.63 /Applications/Brave Browser.app/Contents/MacOS/Brave Browser
```

---

```
❯ ps aux | wc -l
     606
```

---

```
Processes: 606 total, 3 running, 603 sleeping, 2662 threads                                                                                                         17:15:46
Load Avg: 2.15, 2.51, 2.59  CPU usage: 10.41% user, 5.32% sys, 84.26% idle  SharedLibs: 666M resident, 111M data, 48M linkedit.
MemRegions: 310782 total, 5299M resident, 456M private, 1946M shared. PhysMem: 15G used (2011M wired, 1149M compressor), 488M unused.
VM: 254T vsize, 4915M framework vsize, 32140(0) swapins, 86712(0) swapouts. Networks: packets: 13103523/14G in, 5784681/3086M out.
Disks: 15867924/229G read, 7334828/129G written.

PID    COMMAND      %CPU TIME     #TH   #WQ  #PORT MEM    PURG   CMPRS PGRP  PPID  STATE    BOOSTS           %CPU_ME %CPU_OTHRS UID  FAULTS    COW    MSGSENT    MSGRECV
172    WindowServer 44.8 08:39:30 22/1  6    3412  850M   11M    231M  172   1     running  *0[1]            1.79450 2.05331    88   36458061+ 16099  211889190+ 253779268+
50466  zed          41.4 01:53.81 27    7    332-  212M+  384K   0B    50466 1     sleeping *0-[150]         0.73643 0.94970    501  35294+    670    291043+    151823+
0      kernel_task  14.5 04:03:15 534/8 0    0     21M-   0B     0B    0     0     running   0[0]            0.00000 0.00000    0    27837     0      237074273+ 169769776+
39895  Telegram     7.2  41:03.11 33    4    1251  503M   400K   248M  39895 1     sleeping  0[2141]         1.02302 0.00000    501  3055433+  986    20800790+  2028438+
51886  top          3.4  00:00.64 1/1   0    27    7394K  0B     0B    51886 51403 running  *0[1]            0.00000 0.00000    0    6245+     84     776720+    388354+
46084  Brave Browse 2.9  04:19.10 19    1    367   350M   68M    3792K 46078 46078 sleeping *1[116077]       0.00000 0.00000    501  549524    1025   3531172+   1640936+
408    Notification 1.9  03:03.19 6     4    391   74M    0B     27M   408   1     sleeping *0[10225+]       0.06869 0.42959    501  249218+   460    915317+    238823+
583    Rectangle    1.7  01:22.02 3     1    194   22M    0B     8368K 583   1     sleeping *0[14373]        0.34946 0.65411    501  241234+   209    1427066+   448480+
46607  Brave Browse 1.7  00:16.26 19    1    203   90M    0B     19M   46078 46078 sleeping *0[129421+]      0.00000 0.00000    501  57128     1098   153921+    143169+
1114   UniversalCon 1.4  05:54.84 2     1    123   9505K  0B     1728K 1114  1     sleeping *0[1]            0.00000 0.00000    501  1667728+  186    6531865+   3512111+
434    Finder       1.1  06:33.54 8     3    600   122M   0B     39M   434   1     sleeping *0[15187]        0.06243 0.00000    501  1876076+  49525  1962741+   919551+
584    Stats        0.7  41:39.23 8     4    353   77M    32K    46M   584   1     sleeping *0[3295656]      0.00683 0.00000    501  583272    274    22447951+  11547966+
46608  Brave Browse 0.5  01:32.64 21    1    323   438M   0B     77M   46078 46078 sleeping *689+[395]       0.00000 0.00000    501  123105    2223   384523+    213936+
104    logd         0.4  08:48.16 4     3    1929  15M    0B     5184K 104   1     sleeping *0[1]            0.00000 0.00000    0    517131    90     2608722+   2953800+
169    corebrightne 0.1  03:38.32 4     3    139   4689K  0B     1216K 169   1     sleeping *0[1]            0.10763 0.00000    0    566263+   144    1054249+   922969+
2163   com.apple.Am 0.1  03:35.48 3     1    78    4369K  0B     1584K 2163  1     sleeping *1[1]            0.10902 0.00000    0    14081     121    1077162+   624617+
...
```

---

```
❯ sleep 300 &
[1] 51898 # the process id
```

---

```
❯ jobs
[1]  + running    sleep 300 # my job is running :)
```

---

```
❯ ps -ef | grep sleep
  501 51898 51403   0  5:16PM ttys001    0:00.00 sleep 300
  501 51918 51403   0  5:18PM ttys001    0:00.00 grep --color=auto --exclude-dir=.bzr --exclude-dir=CVS --exclude-dir=.git --exclude-dir=.hg --exclude-dir=.svn --exclude-dir=.idea --exclude-dir=.tox --exclude-dir=.venv --exclude-dir=venv sleep
```

---

```
❯ kill 51898
[1]  + 51898 terminated  sleep 300 # killed the job
```

---

```
# create a new job again
❯ sleep 300 &
[1] 51958
```

---

```
# save the PID (Process Identifier) of the most recently started background process into a shell variable called PID
❯ PID=$
# no output
```

---

```
# since i am temporarily on macos, I cannot use `ls /proc/$PID/`
❯ echo $PID
52537
```

---

```
# again, running the closest macos equivalent
❯ ps -p $PID -o pid,ppid,state,%cpu,%mem,rss,vsz,nlwp,comm
kill $PID
ps: nlwp: keyword not found
  PID  PPID STAT  %CPU %MEM    RSS      VSZ COMM
52537 51403 SN     0.0  0.0   1152 410592928 sleep
[1]  + 52537 terminated  sleep 300
```

---

```
Observing:

1. PID is the Process Identifier. It is a unique, numeric ID (an integer) assigned by the Linux kernel to every running process on the system. The PID of my first process was 51898
2. 606 processes running
3. The process state is SN (S - sleeping, N - lower scheduling priority)
```

### Part 3

1. I have 15G of memory used and 1GB free
2. Swap memory is memory on the SSD that the system uses in order to ease pressure on the RAM. Here is my swap usage (1GB)

```
❯ sysctl vm.swapusage
vm.swapusage: total = 1024.00M  used = 182.50M  free = 841.50M  (encrypted)
```

3. My sleep processes uses 1152 MiB of resident memory. This is surprising because it's the sleep processes, it does almost nothing.

### Part 4

1. My root file system is mounted on `/dev/disk3s1s1`
2. I have `/dev/null`. It is a device that is responsible for discarding any data written to it
3. Everything is a file means that my Unix system represents everything (such as the hardware, drivers, etc) as a normal file.

```
❯ ls -l /dev/null
crw-rw-rw-  1 root  wheel  0x3000002 Sep 28 17:45 /dev/null
```

We can observe that the `/dev/null` device is also a file with permissions

### Yes, I finished early!

Let me install htop

```
brew install htop
```

---

Now it exists

```
❯ htop --version
htop 3.5.3-3.5.3
```

---

If I run this command on macos, I will get the following message:

```
❯ sudo dtruss ls
Password:
dtrace: system integrity protection is on, some features will not be available

dtrace: failed to execute ls: Operation not permitted
```

This mean that SIP (System Integrity Protection) is enabled.

If SIP was disabled, I would see something like this

```
SYSCALL(args)                    = return

open(...)                        = 3
fstat64(...)                     = 0
getdirentries64(...)             = ...
write_nocancel(...)              = ...
close(...)                       = 0
```
