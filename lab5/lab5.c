#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int counter = 0; //Counter to count the number of SIGINT signals received
int max_value = 5; // Maximum number of SIGINT signals allowed

// Signal handler for SIGINT (CTRL-C)
void sig_int_handler(int signum) {
    if (counter < max_value) {  //Checking if Maximum Value is not exceeded
        printf(" This is the %d time you pressed ctrl-c\n", ++counter); //Incrementing and prinitng the Counter
        printf("Wait for another signal ...\n");
    } 
        
    else {
      // Maximum value exceeded so deactivating SIGINT handler
        printf("MAXIMUM value exceeded. Deactivating SIGINT handler.\n");
        signal(SIGINT, SIG_DFL);  // Deactivates the signal handler
    }
}

// Signal handler for SIGQUIT (CTRL-\)
void sig_quit_handler(int signum) {
    pid_t pid = fork();  //Creating the Child Process
    if (pid > 0) {
	return;
    } else if (pid == 0) {  // Child process
        printf("I am the child and I am sending a signal\n");
        int kill(getppid(), SIGUSR1);  // Sending SIGUSR1 signal to parent
        exit(EXIT_SUCCESS);
    } else {  
        perror("fork");
        exit(EXIT_FAILURE);

    }
}

// Signal handler for SIGUSR1
void sig_usr1_handler(int signum) {
    printf("Child sent a signal so I guess you are bored, have a great day!\n");
    printf("You Pressed ^c %d times.\n", counter);
    exit(EXIT_SUCCESS);
}



int main() {
    
        // Register signal handlers
        signal(SIGINT, sig_int_handler);
        signal(SIGQUIT, sig_quit_handler);
        signal(SIGUSR1, sig_usr1_handler);

        printf("Wait for another signal ...\n");

  
    bool something = true; //Creates a forever Loop
    while (something) {

        pause();  // Waiting for a signal

    }
    
    

    return 0;
}

