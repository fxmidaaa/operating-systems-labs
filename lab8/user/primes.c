#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int source[2];
  if(pipe(source) < 0) exit(1);
  int pid = fork();
  if(pid < 0) exit(1);

  if(pid == 0){
    close(source[1]);
    int input = source[0];
    for(;;){
      int prime, number, next[2];
      int count = read(input, &prime, sizeof prime);
      if(count == 0){
        close(input);
        exit(0);
      }
      if(count != sizeof prime) exit(1);
      printf("prime %d\n", prime);
      if(pipe(next) < 0) exit(1);
      int child = fork();
      if(child < 0) exit(1);
      if(child == 0){
        close(input);
        close(next[1]);
        input = next[0];
        continue;
      }
      close(next[0]);
      while((count = read(input, &number, sizeof number)) > 0){
        if(count != sizeof number) exit(1);
        if(number % prime != 0){
          if(write(next[1], &number, sizeof number) != sizeof number)
            exit(1);
        }
      }
      if(count < 0) exit(1);
      close(input);
      close(next[1]);
      int status;
      if(wait(&status) < 0) exit(1);
      exit(status);
    }
  }

  close(source[0]);
  for(int number = 2; number <= 35; number++){
    if(write(source[1], &number, sizeof number) != sizeof number)
      exit(1);
  }
  close(source[1]);
  int status;
  if(wait(&status) < 0) exit(1);
  exit(status);
}
