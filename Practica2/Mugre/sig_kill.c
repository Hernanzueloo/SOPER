#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  int sig;
  pid_t pid;
  int ret;

  if (argc != 3) {
    fprintf(stderr, "Usage: %s -<signal> <pid>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  sig = atoi(argv[1] + 1);
  pid = (pid_t)atoi(argv[2]);

  ret= kill(pid,sig);
  if (ret == -1)
      exit(EXIT_SUCCESS);

  exit(EXIT_SUCCESS);
}
