# Lab 5 · The File System with Your Own Hands

**Sergiu Gherasim, FAF-242** · Technical University of Moldova, FCIM · Operating Systems · Laboratory work no. 5

The formatted report is [`OS_Practical_Work_5_Report.tex`](OS_Practical_Work_5_Report.tex) (LaTeX, compiles with `LOGO_UTM.png` in the same folder).  
This lab has no code: only commands, observations and explanations.

## Purpose of the work

- Read the directory tree and recognise the file types from `ls -l`
- Observe inodes behind names, hard links, symbolic links and directory link counts
- Understand the difference between the size of a file and the space it occupies
- Experiment with permission bits on files and directories, the umask, setuid and the sticky bit
- Look at the disks, partitions, mounted file systems and the kernel exposed as files in `/proc`

Everything was done on Ubuntu 24.04 as the normal user `sergiu` (not root):

```bash
whoami                 # sergiu
mkdir -p ~/os-lab5 && cd ~/os-lab5
```

## Part 1. The Tree and the File Types

```text
$ ls -ld /etc /etc/passwd /bin /dev/null /tmp
lrwxrwxrwx   1 root root    7 Apr 22  2024 /bin -> usr/bin
crw-rw-rw-   1 root root 1, 3 Oct 25 18:02 /dev/null
drwxr-xr-x 132 root root 4096 Oct 25 18:05 /etc
-rw-r--r--   1 root root 2914 Sep 30 21:14 /etc/passwd
drwxrwxrwt  15 root root 4096 Oct 25 18:40 /tmp
$ ls -l /dev/vda /dev/tty
brw-rw---- 1 root disk 253, 0 Oct 25 18:02 /dev/vda
crw-rw-rw- 1 root tty    5, 0 Oct 25 18:02 /dev/tty
$ file /usr/bin/ls /etc/passwd /bin
/usr/bin/ls: ELF 64-bit LSB pie executable, ARM aarch64, version 1 (SYSV), dynamically linked, ...
/etc/passwd: ASCII text
/bin:        symbolic link to usr/bin
$ du -sh /var/log 2>/dev/null ; du -sh /usr 2>/dev/null
187M    /var/log
6.3G    /usr
```

### Questions & Answers

**1. For each path in the second line, write its type from the first letter of `ls -l`.**

| Path | Letter | Type |
|---|---|---|
| `/etc` | `d` | directory |
| `/etc/passwd` | `-` | regular file |
| `/bin` | `l` | symbolic link |
| `/dev/null` | `c` | character device |
| `/tmp` | `d` | directory (with sticky bit `t`) |

**2. What is `/bin` on your system: a directory or a link, and to where?**

A symbolic link to `usr/bin` (i.e. `/usr/bin`) — the "merged /usr" layout of modern Ubuntu.

**3. Find one character device and one block device in `/dev`.**

Character device: `/dev/null` (`c`, major/minor `1, 3`). Block device: `/dev/vda`, the disk (`b`, `253, 0`).

**4. Which is bigger, `/var/log` or `/usr`, and by roughly how much?**

`/usr` (6.3 GB) is about 35 times bigger than `/var/log` (187 MB).

## Part 2. Inodes and Links

```text
$ echo hello > notes.txt
$ stat notes.txt
  File: notes.txt
  Size: 6               Blocks: 8          IO Block: 4096   regular file
Device: 253,2   Inode: 1311204     Links: 1
Access: (0664/-rw-rw-r--)  Uid: ( 1000/  sergiu)   Gid: ( 1000/  sergiu)
$ ln notes.txt hard.txt
$ ls -li notes.txt hard.txt
1311204 -rw-rw-r-- 2 sergiu sergiu 6 Oct 25 18:12 hard.txt
1311204 -rw-rw-r-- 2 sergiu sergiu 6 Oct 25 18:12 notes.txt
$ rm notes.txt
$ cat hard.txt ; ls -li hard.txt
hello
1311204 -rw-rw-r-- 1 sergiu sergiu 6 Oct 25 18:12 hard.txt
$ ln -s hard.txt soft.txt
$ ls -li hard.txt soft.txt
1311204 -rw-rw-r-- 1 sergiu sergiu 6 Oct 25 18:12 hard.txt
1311219 lrwxrwxrwx 1 sergiu sergiu 8 Oct 25 18:14 soft.txt -> hard.txt
$ rm hard.txt
$ cat soft.txt ; ls -l soft.txt
cat: soft.txt: No such file or directory
lrwxrwxrwx 1 sergiu sergiu 8 Oct 25 18:14 soft.txt -> hard.txt
$ mkdir d ; ls -ld d
drwxrwxr-x 2 sergiu sergiu 4096 Oct 25 18:15 d
$ mkdir d/x d/y ; ls -ld d
drwxrwxr-x 4 sergiu sergiu 4096 Oct 25 18:15 d
$ echo x > one.txt ; ls -l one.txt ; du -h one.txt
-rw-rw-r-- 1 sergiu sergiu 2 Oct 25 18:16 one.txt
4.0K    one.txt
$ truncate -s 1G big.img ; ls -lh big.img ; du -h big.img
-rw-rw-r-- 1 sergiu sergiu 1.0G Oct 25 18:16 big.img
0       big.img
```

### Questions & Answers

**1. Which number in `stat` is the inode, and which tells how many names the file has?**

`Inode: 1311204` is the inode number; `Links: 1` is the number of names.

**2. After `rm notes.txt`, why could you still read `hard.txt`? What does `rm` really remove?**

Both names pointed to the same inode (link count 2). `rm` calls `unlink()`: it removes only the directory entry and decrements the link count. The inode and data are freed only when the count reaches 0 and no process has the file open.

**3. Why did `cat soft.txt` fail, although `soft.txt` still exists? How big is it and why?**

`soft.txt` is a separate inode that only stores the path `hard.txt`; that name no longer exists, so the link is dangling. Its size is 8 bytes = the length of the string `hard.txt`.

**4. A new directory has 2 links, and 4 after creating two subdirectories. Where does each link come from?**

1 — the name `d` in the parent; 2 — `d/.`; 3 and 4 — `d/x/..` and `d/y/..`.

**5. Why does a 2-byte file use 4.0K, and a 1 GB file use 0?**

Space is allocated in whole 4 KiB blocks, so 2 bytes still take one block. `truncate` only sets the size: the file is sparse and has no blocks allocated.

## Part 3. Permissions

```text
$ echo secret > s.txt
$ chmod 600 s.txt ; ls -l s.txt
-rw------- 1 sergiu sergiu 7 Oct 25 18:20 s.txt
$ chmod 000 s.txt ; cat s.txt
cat: s.txt: Permission denied
$ chmod u+r s.txt ; cat s.txt
secret
$ umask ; touch new.txt ; mkdir newdir ; ls -ld new.txt newdir
0002
-rw-rw-r-- 1 sergiu sergiu    0 Oct 25 18:21 new.txt
drwxrwxr-x 2 sergiu sergiu 4096 Oct 25 18:21 newdir
$ mkdir box ; echo hi > box/f
$ chmod u=r,go= box  ; ls box ; cat box/f     # read only
ls: cannot access 'box/f': Permission denied
f
cat: box/f: Permission denied
$ chmod u=x,go= box  ; ls box ; cat box/f     # execute only
ls: cannot open directory 'box': Permission denied
hi
$ chmod 755 box
$ ls -l /usr/bin/passwd ; ls -ld /tmp
-rwsr-xr-x 1 root root 64152 May 30  2024 /usr/bin/passwd
drwxrwxrwt 15 root root 4096 Oct 25 18:40 /tmp
$ cat /etc/shadow
cat: /etc/shadow: Permission denied
$ ls -l /etc/shadow
-rw-r----- 1 root shadow 1203 Sep 30 21:14 /etc/shadow
```

### Questions & Answers

**1. Write 600, 644, 755 and 700 as rwx strings, and give one real use for each.**

| Octal | String | Use |
|---|---|---|
| 600 | `rw-------` | private files: `~/.ssh/id_ed25519` |
| 644 | `rw-r--r--` | documents/configs readable by all: `/etc/passwd` |
| 755 | `rwxr-xr-x` | programs and public directories: `/usr/bin/ls` |
| 700 | `rwx------` | private directories or scripts: `~/.ssh` |

**2. With `chmod 000` you own the file but cannot read it. Why does ownership not help?**

The kernel checks only the bits of the class you belong to — for the owner, the owner bits, which are `---`. Ownership gives the right to *change* the permissions (`chmod u+r` worked without sudo), not access itself. Only root bypasses the check.

**3. What is your umask, and how does it explain the permissions of `new.txt` and `newdir`?**

`0002`: it removes write for others. Files are requested with 666 → 664 (`rw-rw-r--`), directories with 777 → 775 (`rwxrwxr-x`).

**4. Explain the box experiment: what does `r` allow on a directory, and what does `x` allow?**

`r` allows reading the list of names (so `ls` printed `f`, but could not stat it and `cat` failed). `x` allows traversing the directory to reach the inodes behind the names (so `ls` failed, but `cat box/f` worked because the name was known).

**5. What do the `s` in `/usr/bin/passwd` and the `t` in `/tmp` mean, and why does `passwd` need the `s`?**

`s` is setuid: the program runs with the identity of its owner, root. `passwd` needs it to write the new hash into `/etc/shadow`, which normal users cannot even read. `t` is the sticky bit: everyone can create files in `/tmp`, but only the owner of a file can delete or rename it.

## Part 4. Disks and the Kernel as Files

```text
$ lsblk
NAME   MAJ:MIN RM  SIZE RO TYPE MOUNTPOINTS
sr0     11:0    1 1024M  0 rom
vda    253:0    0   64G  0 disk
├─vda1 253:1    0    1G  0 part /boot/efi
└─vda2 253:2    0   63G  0 part /
$ df -h
Filesystem      Size  Used Avail Use% Mounted on
tmpfs           392M  1.5M  390M   1% /run
/dev/vda2        62G   14G   45G  24% /
tmpfs           2.0G     0  2.0G   0% /dev/shm
tmpfs           5.0M  8.0K  5.0M   1% /run/lock
/dev/vda1       1.1G  6.4M  1.1G   1% /boot/efi
tmpfs           392M  132K  392M   1% /run/user/1000
$ mount | grep " / "
/dev/vda2 on / type ext4 (rw,relatime)
$ grep -c processor /proc/cpuinfo
4
$ head -3 /proc/meminfo
MemTotal:        4010236 kB
MemFree:         1873520 kB
MemAvailable:    3120448 kB
$ head -5 /proc/self/status
Name:   head
Umask:  0002
State:  R (running)
Tgid:   5123
Ngid:   0
$ ls -l /proc/self/fd
lrwx------ 1 sergiu sergiu 64 Oct 25 18:30 0 -> /dev/pts/0
lrwx------ 1 sergiu sergiu 64 Oct 25 18:30 1 -> /dev/pts/0
lrwx------ 1 sergiu sergiu 64 Oct 25 18:30 2 -> /dev/pts/0
lr-x------ 1 sergiu sergiu 64 Oct 25 18:30 3 -> /proc/5131/fd
```

### Questions & Answers

**1. Draw your disk.**

```text
 /dev/vda  (64 GB)
 +-----------------------+------------------------------------+
 | vda1   1 GB  vfat     | vda2   63 GB  ext4                 |
 | mounted on /boot/efi  | mounted on /                       |
 +-----------------------+------------------------------------+
```

**2. How full is your root file system?**

24 %: 14 GB used, 45 GB available out of 62 GB.

**3. How many CPU cores and how much RAM, according to `/proc`?**

4 cores; `MemTotal` 4 010 236 kB ≈ 3.8 GiB.

**4. `/proc/self/status` showed the name of a program. Which one, and why that one?**

`head`. `/proc/self` always points to the process that reads it, and the file was read by `head`, not by the shell.

## Stretch

**(a) Magic numbers**

```text
$ gzip -c /etc/passwd > notes.txt ; file notes.txt ; xxd notes.txt | head -1
notes.txt: gzip compressed data, was "passwd", last modified: Tue Sep 30 21:14:02 2026, from Unix, original size modulo 2^32 2914
00000000: 1f8b 0808 4a1d dc6a 0003 7061 7373 7764  ....J..j..passwd
```

`file` ignores the `.txt` extension and reads the first bytes: `1f 8b` is the gzip magic number (`08` = deflate, next `08` = "original name present").

**(c) Deleting a file that is still open**

```text
$ head -c 200M /dev/zero > bigfile
$ exec 3< bigfile ; rm bigfile ; ls -l /proc/$$/fd
...
lr-x------ 1 sergiu sergiu 64 Oct 25 18:35 3 -> /home/sergiu/os-lab5/bigfile (deleted)
$ df -h . | tail -1
/dev/vda2        62G   15G   44G  25% /
$ exec 3<&- ; df -h . | tail -1
/dev/vda2        62G   14G   45G  24% /
```

The link shows the old path with `(deleted)`. The space is freed only after `exec 3<&-` closes the last descriptor.

## Conclusion

In this lab I checked the file system lecture on a real Linux system. I saw that names and inodes are separate: `rm` only removes a directory entry, hard links keep the data alive, symbolic links are small files holding a path, and directories collect a link from every subdirectory's `..`. I also saw that size and allocated space differ because of blocks and sparse files.

The permission experiments showed how the kernel picks exactly one class of bits, that ownership only lets you change permissions, what `r` and `x` mean on a directory, and how umask, setuid and the sticky bit work. Finally, `lsblk`, `df` and `/proc` showed how disks, partitions and even the kernel's own state appear as parts of the single directory tree.
