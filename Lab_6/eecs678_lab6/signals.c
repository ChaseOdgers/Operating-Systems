#include <stdio.h>     /* standard I/O functions                         */
#include <stdlib.h>    /* exit                                           */
#include <string.h>    /* memset                                         */
#include <stdbool.h>   /* using bool data type			                     */
#include <unistd.h>    /* standard unix functions, like getpid()         */
#include <signal.h>    /* signal name macros, and the signal() prototype */
#include <sys/types.h> /*needed for pid_t                                */

#define CTRL_C_THRESHOLD  5

/* variable to keep count of ctrl_c presses */
int ctrl_c_count = 0;

/* flag to keep track of user response after exit prompt */
int got_response = 0;
int pause(void);
/* integer array to be used as pipe */
int p[2];


void check_count(){
  char answer[30];

  /* Check if threshold was reached */
  if (ctrl_c_count >= CTRL_C_THRESHOLD) {

    /* Prompt the user to tell us if to close the pipe p's read end */
    printf("\nClose pipe's read end? [Y/n]: ");
    fflush(stdout);

    /* If user hasn't responded in 5 seconds, exit the program */
    //YOUR CODE
    alarm(5);


    fgets(answer, sizeof(answer), stdin);

    /* If we have reached this point, the user has already responded */
    if (answer[0] == 'n' || answer[0] == 'N') {
      printf("\nPipe p's read end is still open. Continuing\n");
      fflush(stdout);

      /* Keep track of the fact that the user has responded */
      got_response = 1;

      /* Reset the ctrl_c_count counter */
      ctrl_c_count = 0;
    }
    else {
      printf("\nClosing pipe p's read end p[0]...\n");
      fflush(stdout);

      /* Close the read end of the pipe p */
      //YOUR CODE
      close(p[0]);
    }

    printf("\nWriting to the pipe p's write end\n");
    write(p[1], "ABC", sizeof("ABC"));
  }
}

/* the SIGPIPE signal handler */
void catch_pipe(int sig_num){
	printf("\nSIGPIPE received - exiting...\n");

	exit(-1);
}

/* the Ctrl-C signal handler */
void catch_int(int sig_num)
{
  /* Increment the ctrl_c_counter and call check_count function */
  //YOUR CODE
  ctrl_c_count++;
  check_count();
}

/* Implement alarm handler - (The program should exit after receiving the alarm) */
//YOUR CODE
void catch_alarm(int sig_num)
{
  printf("\nNo response from the user! exiting...\n");
  exit(-1);
}


int main(int argc, char* argv[])
{
  //while(got_response != -1){
    pipe(p);

    struct sigaction sa1;
    struct sigaction sa2;
    struct sigaction sa3;
    sigset_t mask_set;  /* used to set a signal masking set. */

    /* clear the memory at sa - by filling up the memory location at sa with the value 0 till the size of sa - execute  "man memset" from the terminal */
    //YOUR CODE
    memset(&sa1, 0, sizeof(sa1));
    memset(&sa2, 0, sizeof(sa2));
    memset(&sa3, 0, sizeof(sa3));

    /* setup mask_set */
    //YOUR CODE
    sigfillset(&mask_set);
    sa1.sa_handler=catch_pipe;
    sa2.sa_handler=catch_int;
    sa3.sa_handler=catch_alarm;


    /* ensure in the mask_set that the alarm signal does not get blocked while in another signal handler */
    //YOUR CODE
    sigdelset(&mask_set, SIGALRM);


    /* set signal handlers for SIGPIPE, SIGINT, and SIGALRM */
    //YOUR CODE
      sigaction(SIGPIPE, &sa1, NULL);
      sigaction(SIGINT, &sa2, NULL);
      sigaction(SIGALRM, &sa3, NULL);

    /* ensure that the program keeps running to receive the signals */
    //YOUR CODE
    while(-1 != 1)
    {
      pause();
    }
}
