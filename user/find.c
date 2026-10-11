#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"
#include "user/user.h"

char *
basename(char *path)
{
  char *p = path + strlen(path);

  while (p > path && *(p - 1) != '/')
    p--;

  return p;
}

void
runexec(char *path, char **cmd, int ncmd)
{
  char *args[MAXARG];
  int i;

  if (ncmd + 1 >= MAXARG) {
    fprintf(2, "find: too many arguments\n");
    return;
  }

  for (i = 0; i < ncmd; i++)
    args[i] = cmd[i];

  args[i++] = path;
  args[i] = 0;

  int pid = fork();

  if (pid < 0) {
    fprintf(2, "find: fork failed\n");
    return;
  }

  if (pid == 0) {
    exec(args[0], args);
    fprintf(2, "find: exec failed\n");
    exit(1);
  }

  wait(0);
}

void
find(char *path, char *name, char **cmd, int ncmd)
{
  int fd;
  char buf[512], *p;
  struct stat st;
  struct dirent de;

  if ((fd = open(path, O_RDONLY)) < 0) {
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0) {
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  if (strcmp(basename(path), name) == 0) {
    if (cmd)
      runexec(path, cmd, ncmd);
    else
      printf("%s\n", path);
  }

  if (st.type == T_DIR) {
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
      fprintf(2, "find: path too long\n");
      close(fd);
      return;
    }

    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';

    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
      if (de.inum == 0)
        continue;

      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;

      if (strcmp(p, ".") == 0 || strcmp(p, "..") == 0)
        continue;

      find(buf, name, cmd, ncmd);
    }
  }

  close(fd);
}

int
main(int argc, char *argv[])
{
  if (argc < 3) {
    fprintf(2, "usage: find path name [-exec cmd ...]\n");
    exit(1);
  }

  if (argc == 3) {
    find(argv[1], argv[2], 0, 0);
  } else {
    if (argc < 5 || strcmp(argv[3], "-exec") != 0) {
      fprintf(2, "usage: find path name [-exec cmd ...]\n");
      exit(1);
    }

    find(argv[1], argv[2], &argv[4], argc - 4);
  }

  exit(0);
}