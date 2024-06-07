#include <stdio.h> 
#include <stdlib.h> 
#include <signal.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/wait.h>
// Explicit declaration of the kill function 
extern int kill(pid_t pid, int sig); int count = 0;
#define maxCount 5
void sigint(int signum) {
    printf("Waiting for another signal ...\n");
    if (count < maxCount)
    {
        count++;
        if (count % 10 == 1)
            printf("^C This is the %dst time you pressed ctrl-c\n", count);
        else if (count % 10 == 2)
            printf("^C This is the %dnd time you pressed ctrl-c\n", count);
        else if (count % 10 == 3)
            printf("^C This is the %drd time you pressed ctrl-c\n", count);
        else
            printf("^C This is the %dth time you pressed ctrl-c\n", count);
    }
    else
    {
        printf("MAX count exceeded. Deactivating SIGINT handler.\n");
        signal(SIGINT, SIG_DFL); // Set SIGINT handler to default
        printf("You pressed ^c 5 times.\n");
        exit(0); // Terminate the program
    }
}
void sigquit(int signum) {
    pid_t forking = fork();
    if (forking < 0)
    {
        perror("Sorry, fork failed!");
        exit(1);
    }
    else if (forking == 0)
    {
        printf("^\\ I am the child and I am sending a signal\n");
        if (kill(getppid(), SIGUSR1) == -1) {
            perror("kill failed");
            exit(1);
        }
        exit(0);
    }
    else
    {
        int status;
        waitpid(forking, &status, 0);
        printf("Child sent a signal so I guess you are bored, have a great day!\n");
        exit(0); // Terminate the program after the child process finishes
    }
}
void sigusr1(int signum) {
    printf("Program is over.\n");
    exit(0);
}
int main() {
    signal(SIGINT, sigint);
    signal(SIGQUIT, sigquit);
    signal(SIGUSR1, sigusr1);
    while (1)
    {
        pause();
    }
    return 0;
}
