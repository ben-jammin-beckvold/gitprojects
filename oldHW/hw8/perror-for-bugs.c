#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

// This purposely has bugs, to illustrate perror()
int main() {
  int fd = open("/etc/passwd", O_DIRECTORY, 0);
  if (fd == -1)
    perror("open");
  char buf[100];
  int num = read(fd, buf, 100);
  if (num == -1)
    perror("read");
  int rc = close(5); // rc: return code
  if (num == -1)
    perror("close");
  return 0;
}
