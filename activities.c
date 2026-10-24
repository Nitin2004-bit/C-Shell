#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>

#define MY_MAX_INPUT 256

// Function to print process details if parent PID matches the shell's PID
void print_process_state(pid_t pid, pid_t shell_pid) {
    char status_file[MY_MAX_INPUT];
    snprintf(status_file, sizeof(status_file), "/proc/%d/status", pid);

    FILE* fd = fopen(status_file, "r");
    if (fd == NULL) {
        // If the process no longer exists or can't be accessed
        return;
    }

    char line[MY_MAX_INPUT];
    char state[16], command[MY_MAX_INPUT];
    int ppid = 0;
    
    while (fgets(line, sizeof(line), fd)) {
        if (strncmp(line, "Name:", 5) == 0) {
            sscanf(line, "Name:\t%s", command);
        } else if (strncmp(line, "State:", 6) == 0) {
            sscanf(line, "State:\t%s", state);
        } else if (strncmp(line, "PPid:", 5) == 0) {
            sscanf(line, "PPid:\t%d", &ppid);
        }
    }

    fclose(fd);

    // Check if the parent PID matches the shell's PID
    if (ppid == shell_pid) {
        // Convert state to "Running" or "Stopped"
        char* process_state = (strstr(state, "R")|| strstr(state,"S") || strstr(state,"Z")) ? "Running" : "Stopped";
        printf("[%d] : %s - %s\n", pid, command, process_state);
    }
}

void activity() {
    pid_t shell_pid = getpid();  // Get the PID of the shell
    
    DIR* proc_dir = opendir("/proc");
    if (proc_dir == NULL) {
        perror("Failed to open /proc directory");
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(proc_dir)) != NULL) {
        // Check if the entry is a number (PID)
        if (isdigit(*entry->d_name)) {
            pid_t pid = atoi(entry->d_name);
            print_process_state(pid, shell_pid);
        }
    }

    closedir(proc_dir);
}
