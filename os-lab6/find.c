#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

// find <dir> <name> [-n]
// prints every path under <dir> whose last component is <name>;
// with -n it also prints the inode number of each match.

int show_inode = 0;

// return a pointer to the last component of path
char *
lastname(char *path)
{
  char *p;

  for(p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  return p + 1;
}

void
find(char *path, char *name)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, O_RDONLY)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  if(st.type != T_DIR){
    // find was started on a file: only compare its own name
    if(strcmp(lastname(path), name) == 0){
      if(show_inode)
        printf("%s %d\n", path, st.ino);
      else
        printf("%s\n", path);
    }
    close(fd);
    return;
  }

  if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
    fprintf(2, "find: path too long\n");
    close(fd);
    return;
  }
  strcpy(buf, path);
  p = buf + strlen(buf);
  *p++ = '/';

  while(read(fd, &de, sizeof(de)) == sizeof(de)){
    if(de.inum == 0)                      // free directory slot
      continue;
    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = 0;
    if(strcmp(p, ".") == 0 || strcmp(p, "..") == 0)
      continue;                           // never walk back up the tree
    if(stat(buf, &st) < 0){
      fprintf(2, "find: cannot stat %s\n", buf);
      continue;
    }
    if(strcmp(p, name) == 0){
      if(show_inode)
        printf("%s %d\n", buf, st.ino);
      else
        printf("%s\n", buf);
    }
    if(st.type == T_DIR)
      find(buf, name);                    // recurse into subdirectories
  }
  close(fd);                              // only 16 open files per process
}

int
main(int argc, char *argv[])
{
  if(argc < 3 || argc > 4 || (argc == 4 && strcmp(argv[3], "-n") != 0)){
    fprintf(2, "usage: find <dir> <name> [-n]\n");
    exit(1);
  }
  if(argc == 4)
    show_inode = 1;
  find(argv[1], argv[2]);
  exit(0);
}
