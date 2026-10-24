#include "header.h"

void print_into_file(pid_t pid) {
    char cmmd[MY_MAX_INPUT];
    snprintf(cmmd, sizeof(cmmd), "/proc/%d/status", pid);
    FILE* fd = fopen(cmmd, "r");
    if (fd == NULL) {
        perror("error opening file");
        return;
    }

    char line[MY_MAX_INPUT];
    char status[32], vm_size[32];
    int pgid = 0;
    while (fgets(line, sizeof(line), fd)) {
        if (strncmp(line, "Pid:", 4) == 0) {
            // printf("PID: %s", line + 5);
        } else if (strncmp(line, "State:", 6) == 0) {
            sscanf(line, "State:\t%s", status);
            
            
        } else if (strncmp(line, "Tgid:", 5) == 0) {
            sscanf(line, "Tgid:\t%d", &pgid);
            
        } else if (strncmp(line, "VmSize:", 7) == 0) {
            sscanf(line, "VmSize:\t%s", vm_size);
            
        }
    }
    fclose(fd);
    pid_t fb=tcgetpgrp(STDIN_FILENO);
            // printf("%d\n",(int)fb);
    if(fb==-1){
        perror("tcgetpgrp error");
    }else{
        if(pgid!=fb){
            printf("Process Status: %s-\n", status);
        }else{
            printf("Process Status: %s+\n", status);
        }
    }
    printf("Process Group: %d\n", pgid);
    printf("Virtual Memory: %s kB\n", vm_size);

    // Fetch the executable path
    snprintf(cmmd, sizeof(cmmd), "/proc/%d/exe", pid);
    char exe_path[MY_MAX_INPUT];
    ssize_t len = readlink(cmmd, exe_path, sizeof(exe_path) - 1);
    if (len != -1) {
        exe_path[len] = '\0';
        printf("Executable Path: %s\n", exe_path);
    } else {
        perror("Error reading executable path");
    }
}

void proclore(char* command) {
    pid_t my_pid;
    if (!strcmp(command, ".")) {
        my_pid = getpid();
    } else {
        my_pid = atoi(command);
    }
    printf("Process PID: %d\n", my_pid);
    print_into_file(my_pid);
}


