#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/param.h"
#include "kernel/fcntl.h"
#include "user/user.h"

static char *target;
static int show_inode;
static int failed;

static void
walk(char *path)
{
  char buf[MAXPATH], name[DIRSIZ + 1];
  char *base = path;
  struct stat st;
  struct dirent de;
  int fd, count, len;

  if(stat(path, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    failed = 1;
    return;
  }
  for(char *p = path; *p; p++)
    if(*p == '/')
      base = p + 1;
  if(strcmp(base, target) == 0){
    if(show_inode)
      printf("%s: inode %u\n", path, st.ino);
    else
      printf("%s\n", path);
  }
  if(st.type != T_DIR)
    return;
  if((fd = open(path, O_RDONLY)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    failed = 1;
    return;
  }
  while((count = read(fd, &de, sizeof(de))) == sizeof(de)){
    if(de.inum == 0)
      continue;
    memmove(name, de.name, DIRSIZ);
    name[DIRSIZ] = 0;
    if(strcmp(name, ".") == 0 || strcmp(name, "..") == 0)
      continue;
    len = strlen(path);
    int slash = len > 0 && path[len - 1] != '/';
    if(len + slash + strlen(name) + 1 > sizeof(buf)){
      fprintf(2, "find: path too long under %s\n", path);
      failed = 1;
      continue;
    }
    strcpy(buf, path);
    if(slash)
      buf[len++] = '/';
    strcpy(buf + len, name);
    walk(buf);
  }
  if(count != 0){
    fprintf(2, "find: cannot read directory %s\n", path);
    failed = 1;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  char start[MAXPATH];
  int len;

  if((argc != 3 && argc != 4) ||
     (argc == 4 && strcmp(argv[3], "-n") != 0)){
    fprintf(2, "usage: find <dir> <name> [-n]\n");
    exit(1);
  }
  if(strlen(argv[1]) >= sizeof(start)){
    fprintf(2, "find: starting path too long\n");
    exit(1);
  }
  strcpy(start, argv[1]);
  len = strlen(start);
  while(len > 1 && start[len - 1] == '/')
    start[--len] = 0;
  target = argv[2];
  show_inode = argc == 4;
  walk(start);
  exit(failed);
}
