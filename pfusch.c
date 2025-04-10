// made by manuel 10.04.25


#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <time.h>

typedef struct command_line_arguments {
  int i;
  char const *s;
  bool b;
  } cli_args;

  cli_args parse_command_line(int const argc, char * argv [])
{
      cli_args args = {0, NULL, false};
      int optgot = -1;
do
{
  optgot = getopt(argc, argv, "i:s:b");
  switch (optgot) {
    case 'i':
      args.i = atoi(optarg);
    break;
    case 's':
      args.s = optarg;
      break;
      case 'b':
        args.b = false;
        break;
        case '?':
          printf("Usage: %s -i <number> -s <string> -b\n", argv[0]);
          exit(EXIT_FAILURE);
                 }
} while (optgot != -1);
    return args;
    }

    int child_labour() {
      printf("I'm %d, the child of %d\n", getpid(), getppid());
      srand(time(NULL));
      sleep(rand() % 5);
      printf("[%d] Doing some work....\n", getpid());
      sleep(3);
      printf("[%d] Jobs done!\n", getpid());
    printf("[%d] Bringing coal to %d.....\n", getpid(), getppid());

    return getpid();


}



int main(int argc, char * argv[]) {
  //cli_args const args = parse_command_line(argc, argv);

  //printf("i: %d, s: %s, b: %d\n", args.i, args.s, args.b);

  //TODO: Prozess starten
  //execlp("ls","ls","-l", NULL);

  printf("[%d] Sending a child into the mines...\n", getpid());

  for (int i = 0; i < 10; i++) {
    pid_t forked = fork();
    if (forked == 0) {
      return child_labour();
    }
  }


    printf("[%d] Enjoying some brandy...\n", getpid());
    printf("[%d] Where the fudge is my coal?????\n", getpid());

    int wstatus = 0;
    pid_t const waited = wait(&wstatus);
    if (WIFEXITED(wstatus)) {
      printf("[%d] Child exited normally with return code %d\n", getpid(), waited, WEXITSTATUS(waited));


    } else {
      printf("[%d] Child %d terminated abnormally\n", getpid(), waited);
      }

      printf("All children have returned\n");

  //printf("my PID is %d, fork returned %d, I'm a child of %d\n", getpid(), forked, getppid());



  //return 0;

}
