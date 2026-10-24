#include "header.h"
#define MAX_PIPES 10
//assumption cho log & | wc
    //   2       2      15
    // take the input in which is process form the printing it exited normally as well
void execute_pipe(char* cmds[], int cmd_count) {
    int pipes[MAX_PIPES][2];
    
    // Create the pipes
    for (int i = 0; i < cmd_count - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe not established");
            return;
        }
    }

    int i = 0;
    while (i < cmd_count) {
        pid_t pid = fork();
        if (pid == 0) { // Child process
            // Setup input redirection
            if (i > 0) { // If not the first command, read from the previous pipe
                dup2(pipes[i - 1][0], STDIN_FILENO);
                close(pipes[i - 1][0]);
            }

            // Setup output redirection
            if (i < cmd_count - 1) { // If not the last command, write to the next pipe
                dup2(pipes[i][1], STDOUT_FILENO);
                close(pipes[i][1]);
            }

            // Close all pipes in the child process
            for (int j = 0; j < cmd_count - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            // Execute the command
            if (strstr(cmds[i], ">") || strstr(cmds[i], "<") || strstr(cmds[i], ">>")) {
                io_handler(cmds[i]); // Handle redirection if necessary
            } else {
                // printf("cmd=%s1\n",cmds[i]);
                exect(cmds[i]); // Execute the command
            }

            exit(EXIT_FAILURE); // In case exec fails
        } else if (pid < 0) {
            perror("fork");
            return;
        }

        i++; // Move to the next command
    }

    // Close all pipes in parent process
    for (int i = 0; i < cmd_count - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // Wait for all child processes to finish
    i = 0;
    while (i < cmd_count) {
        wait(NULL);
        i++;
    }
}

void pipe_handler(char* command) {
    fflush(stdout);
    fflush(stdin);
    char command_buffer[MY_MAX_INPUT];
    strncpy(command_buffer, command, sizeof(command_buffer));
    command_buffer[sizeof(command_buffer) - 1] = '\0';
    
    char *cmds[MAX_PIPES + 1];
    int cmd_count = 0;
    int pip_count = 0;

    // Count number of pipes
    for (int i = 0; i < strlen(command_buffer); i++) {
        if (command_buffer[i] == '|') {
            pip_count++;
        }
    }

    // Tokenize the commands using '|'
    char* tok = strtok(command, "|");
    while (tok != NULL && cmd_count < MAX_PIPES + 1) {
        cmds[cmd_count++] = tok;
        tok = strtok(NULL, "|\n");
    }

    if ((pip_count + 1) != cmd_count) {
        perror("invalid pipe");
        return;
    }

    // Execute the commands with pipes
    execute_pipe(cmds, cmd_count);
}
