#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_COMMAND_LENGTH 50
#define MAX_VERB_LENGTH 10
#define MAX_PARAMETER_LENGTH 50

void help() {
    printf("-----------------***********************---------------------\n");
    printf(" Valid verbs are:\n");
    printf(" Help:              Displays this text\n");
    printf(" diskuse:           Shows the amount of disk being used in the current directory.\n");
    printf(" drives:            Lists all of the ext4 mounted partitions as well as their size and free space.\n");
    printf(" chklog [username]: Scans the access log for a specific username. If not present, it dumps the whole log file.\n");
    printf(" jobtree[username]: Shows the processes running related to a specific username.\n");
    printf(" logout:            Exits this shell and returns to your original shell.\n");
    printf(" NOTE: elements in square brackets are optional.\n");
}
 
void diskuse(){
    system("du -sch");
}

void drives() {
    system("df -h -t ext4");
}

void chklog(const char *username) {
    char command[MAX_COMMAND_LENGTH];
    if (username == NULL) {
        // If username is not provided, use the environment variable USER
        snprintf(command, sizeof(command), "grep -e $USER /home/COIS/3380/secure");
    } else {
        // Remove newline character from the end of the username
        char cleanedUsername[MAX_VERB_LENGTH];
        snprintf(cleanedUsername, sizeof(cleanedUsername), "%.*s", (int)strcspn(username, "\n"), username);

        snprintf(command, sizeof(command), "grep -e %s /home/COIS/3380/secure", cleanedUsername);
    }
    system(command);
}
void jobtree(const char *username) {
    char command[100];
    if (username == NULL) {
        snprintf(command, sizeof(command), "ps -o \"user:32,pid,stime,tty,cmd\" -U $USER --forest");
    } else {
        snprintf(command, sizeof(command), "ps -o \"user:32,pid,stime,tty,cmd\" -U %s --forest", username);
    }
    system(command);
}

void executeCommand(char *command, char *parameter) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) { // Child process
        if (strcmp(command, "help") == 0) {
            help();
        } else {
            char *args[4];
            args[0] = strdup("/bin/bash");
            args[1] = strdup("-c");
            args[2] = strdup(command);

            if (parameter != NULL) {
                args[3] = strdup(parameter);
                args[4] = NULL;
            } else {
                args[3] = NULL;
            }

            execv(args[0], args);
            perror("Execv failed");
            exit(EXIT_FAILURE);
        }
    } else { // Parent process
        wait(NULL);
    }
}

int main() {
    char input[MAX_COMMAND_LENGTH];
    char command[MAX_COMMAND_LENGTH];
    char parameter[MAX_PARAMETER_LENGTH];

    printf("Welcome to the VI-SHELL  Type 'help' for a list of valid commands.\n");

    while (1) {
        printf(">> ");
        fgets(input, sizeof(input), stdin);
        sscanf(input, "%s %s", command, parameter);

        if (strcmp(command, "logout") == 0) {
            printf("Exiting the shell. Goodbye!\n");
            break;
        } else {
            executeCommand(command, parameter);
        }
    }

    return 0;
}
