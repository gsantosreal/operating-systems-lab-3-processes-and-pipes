#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

/**
 * Executes the command "cat scores | grep Lakers".  In this quick-and-dirty
 * implementation the parent doesn't wait for the child to finish and
 * so the command prompt may reappear before the child terminates.
 *
 */

int main(int argc, char **argv)
{
  if (argc < 2)
  {
    fprintf(stderr, "Usage: %s <search_term>\n", argv[0]);
    exit(1);
  }
  
  int pipefd1[2];
  int pipefd2[2];

  pid_t pid1, pid2;

  char *cat_args[] = {"cat", "scores", NULL};
  char *grep_args[] = {"grep", argv[1], NULL};
  char *sort_args[] = {"sort", NULL};

  if (pipe(pipefd1) == -1)
  {
    perror("Pipe 1 creation failed");
    exit(1);
  }
  if (pipe(pipefd2) == -1)
  {
    perror("Pipe 2 creation failed");
    exit(1);
  }

  pid1 = fork();

  if (pid1 < 0)
  {
    perror("Fork 1 failed");
    exit(1);
  }
  if (pid1 == 0)
  {
    dup2(pipefd1[1], 1);

    close(pipefd1[0]);
    close(pipefd1[1]);
    close(pipefd2[0]);
    close(pipefd2[1]);

    execvp("cat", cat_args);
    perror("execvp cat failed");
    exit(1);
  }
  
  pid2 = fork();
  
  if (pid2 < 0)
  {
    perror("Fork 2 failed");
    exit(1);
  }

  if (pid2 == 0)
  {
    dup2(pipefd1[0], 0);
    dup2(pipefd2[1], 1);

    close(pipefd1[0]);
    close(pipefd1[1]);
    close(pipefd2[0]);
    close(pipefd2[1]);

    execvp("grep", grep_args);
    perror("execvp grep failed");
    exit(1);
  }

  dup2(pipefd2[0], 0);

  close(pipefd1[0]);
  close(pipefd1[1]);
  close(pipefd2[0]);
  close(pipefd2[1]);

  execvp("sort", sort_args);
  perror("execvp sort failed");
  exit(1);

  return 0;
}