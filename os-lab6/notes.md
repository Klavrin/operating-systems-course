# Lab 6 · Inside the xv6 File System

**Sergiu Gherasim, FAF-242** · Technical University of Moldova, FCIM · Operating Systems · Laboratory work no. 6

The formatted report is [`OS_Practical_Work_6_Report.tex`](OS_Practical_Work_6_Report.tex) (LaTeX, compiles with `LOGO_UTM.png` in the same folder).  
The source files for this lab are [`mystat.c`](mystat.c), [`find.c`](find.c) and the [`Makefile`](Makefile) with the new `UPROGS` lines.

## Purpose of the work

- Decode what the xv6 `ls` prints: type, inode number and size of every directory entry
- Write `mystat`, which prints the fields of `struct stat`, and use it to watch hard links at work
- Find the limits of the xv6 file system in `kernel/fs.h` and in the disk layout printed by `mkfs`
- Write `find`, which walks the directory tree recursively (MIT 6.1810 util lab)

New programs were added to `UPROGS`:

```make
UPROGS=\
	$U/_cat\
	...
	$U/_sync\
	$U/_mystat\
	$U/_find\
```

## Part 1. Read What xv6 Shows You

```text
$ ls
.              1 1 1024
..             1 1 1024
README         2 2 2441
cat            2 3 35376
echo           2 4 34232
...
mystat         2 25 34928
find           2 26 36216
console        3 27 0
$ mkdir a
$ ls a
.              1 28 32
..             1 1 1024
```

Each line is: name, type, inode number, size in bytes.

### Questions & Answers

**1. What do types 1, 2 and 3 mean (`kernel/stat.h`)?**

`1 = T_DIR` (directory), `2 = T_FILE` (regular file), `3 = T_DEVICE` (device, here only `console`).

**2. Which inode number does the root directory have, and why are `.` and `..` the same there?**

Inode 1 (`ROOTINO`). `.` points to the directory itself, and the root has no parent, so `mkfs` makes its `..` point to itself too.

**3. An empty directory `a` has size 32. How big is one entry, and which two entries does `a` contain?**

One `struct dirent` = 2-byte `inum` + 14-byte name (`DIRSIZ`) = 16 bytes. `a` contains `.` (inode 28) and `..` (inode 1): 2 × 16 = 32.

## Part 2. Write mystat

```c
char *
typename(int t)
{
  switch(t){
  case T_DIR:
    return "directory";
  case T_FILE:
    return "file";
  case T_DEVICE:
    return "device";
  }
  return "?";
}
...
    if(stat(argv[i], &st) < 0){
      fprintf(2, "mystat: cannot stat %s\n", argv[i]);
      continue;
    }
    printf("%s: %s, inode %d, links %d, size %d bytes\n",
           argv[i], typename(st.type), st.ino, st.nlink, (int)st.size);
```

```text
$ mystat README . console
README: file, inode 2, links 1, size 2441 bytes
.: directory, inode 1, links 2, size 1024 bytes
console: device, inode 27, links 1, size 0 bytes
$ ln README r2
$ mystat README r2
README: file, inode 2, links 2, size 2441 bytes
r2: file, inode 2, links 2, size 2441 bytes
$ rm README
$ mystat r2
r2: file, inode 2, links 1, size 2441 bytes
$ cat r2 | wc
49 324 2441
```

(`.` has 2 links because directory `a` from Part 1 has a `..` pointing to the root.)

### Questions & Answers

**1. Paste the outputs after `ln` and after `rm`. What changed and what did not?**

After `ln`, both names show inode 2 and the link count became 2. After `rm`, only `r2` remains, with links 1. The inode number, type, size and content did not change — only directory entries and `nlink` changed.

**2. Find the line in `kernel/sysfile.c` (`sys_unlink`) that decrements the link count. When are the data blocks freed?**

`ip->nlink--;` (line 244). `sys_unlink` itself frees nothing: the blocks are freed in `iput()` (`kernel/fs.c`), which calls `itrunc()` when the last in-memory reference is dropped **and** `nlink == 0`.

**3. Which fields does an xv6 inode (`struct dinode`) have?**

`type`, `major`, `minor`, `nlink`, `size`, `addrs[NDIRECT+1]` (12 direct + 1 indirect block). No owner, no permissions, no timestamps, no symlink type. 64 bytes per inode.

## Part 3. Measure the Limits

```text
$ grep -n "define BSIZE\|define NDIRECT\|define NINDIRECT\|define DIRSIZ" kernel/fs.h
5:#define BSIZE   1024 // block size
26:#define NDIRECT   12
27:#define NINDIRECT (BSIZE / sizeof(uint))
54:#define DIRSIZ 14
$ make clean ; make fs.img 2>&1 | grep nmeta
nmeta 47 (boot, super, log blocks 31, inode blocks 13, bitmap blocks 1) blocks 1953 total 2000
```

```text
$ mkdir averyveryverylongname
$ ls
...
averyveryveryl 1 28 32
```

### Questions & Answers

**1. What does `ls` show for `averyveryverylongname`, and which constant explains it?**

`averyveryveryl` — the name is cut to 14 characters because of `DIRSIZ = 14`.

**2. Compute the largest file xv6 can hold.**

`NINDIRECT = 1024 / 4 = 256`, `MAXFILE = 12 + 256 = 268` blocks, `268 × 1024 = 274 432` bytes (268 KiB).

**3. From the `nmeta` line: how many blocks in total, and how many for the log, the inodes and the data?**

| Region | Blocks |
|---|---|
| boot + superblock | 2 |
| log (1 header + 30) | 31 |
| inodes (200 / 16 + 1) | 13 |
| bitmap | 1 |
| **data** | **1953** |
| **total** | **2000** |

## Part 4. Write find

`find.c` is based on `user/ls.c`: it reads `struct dirent` entries, skips `inum == 0`, never recurses into `.` and `..`, compares names with `strcmp`, recurses into subdirectories and closes every directory it opens (only 16 open files per process). See [`find.c`](find.c).

```text
$ mkdir a
$ mkdir a/b
$ echo x > a/b/r2
$ echo y > a/r2
$ ln cat r2
$ find . r2
./a/b/r2
./a/r2
./r2
$ find a b
a/b
```

## Stretch

`find . r2 -n` also prints the inode number:

```text
$ find . r2 -n
./a/b/r2 30
./a/r2 31
./r2 3
$ mystat cat r2
cat: file, inode 3, links 2, size 35376 bytes
r2: file, inode 3, links 2, size 35376 bytes
```

Only `./r2` is the same file as `cat` (inode 3, 2 links). The other two are independent files created by `echo`.

## Conclusion

In this lab I looked at the xv6 file system both from the shell and from the source code. A directory turned out to be literally an array of 16-byte entries, the root is inode 1 and its own parent, and `mystat` showed that `ln` and `rm` only change directory entries and the link count, while blocks are freed in `iput` only when the count reaches zero — the same rule as on Linux in Lab 5.

The limits in `kernel/fs.h` show how deliberately small xv6 is: 14-character names, 268 KiB files and a 2000-block disk with 47 metadata blocks. Turning `ls` into a recursive `find` taught me to walk the tree safely: skip free slots and `.`/`..`, and close every descriptor. The stretch task confirmed once more that a name is only a pointer to an inode.
