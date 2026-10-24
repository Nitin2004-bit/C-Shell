#include "header.h"
#include <signal.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>

void fg(int pid) {
    // printf("in\n");
    
    // Check if the process exists
    if (kill(pid, 0) == -1) { 
        printf("No such process found\n");
        return;
    }

    // Send SIGCONT to continue the process if it's stopped
    if (kill(pid, SIGCONT) == -1) {
        perror("kill");
        printf("Failed to continue process\n");
        return;
    }

    // Wait for the process to complete
    int status;
    if (waitpid(pid, &status, WUNTRACED) == -1) {
        perror("waitpid");
        return;
    }

    // Check if the process terminated normally or was stopped
    if (WIFEXITED(status)) {
        printf("Process %d done, exited with status %d\n", pid, WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("Process %d terminated by signal %d\n", pid, WTERMSIG(status));
    } else if (WIFSTOPPED(status)) {
        printf("Process %d was stopped by signal %d\n", pid, WSTOPSIG(status));
    }
}

void bgs(int pid){
    // Check if process exists
    if (kill(pid, 0) == -1) { // Check if process exists
        printf("No such process found\n");
        return;
    }

    // Send SIGCONT to continue the process if it's stopped
    if (kill(pid, SIGCONT) == -1) {
        perror("kill");
        printf("No such process found\n");
        return;
    }

    // Detach process from terminal and let it run in the background
    if (setpgid(pid, pid) == -1) { // Set process group to detach from terminal
        perror("setpgid");
        return;
    }
}
