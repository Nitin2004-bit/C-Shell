#include"header.h"

// Function to get the most recent PID
int get_recent_pid() {
    DIR *proc_dir;
    struct dirent *entry;
    int max_pid = -1;
    
    // Open the /proc directory
    proc_dir = opendir("/proc");
    if (proc_dir == NULL) {
        perror("opendir failed");
        return -1;
    }

    // Read each entry in the /proc directory
    while ((entry = readdir(proc_dir)) != NULL) {
        // Only consider directories with numeric names (process IDs)
        if (isdigit(entry->d_name[0])) {
            int pid = atoi(entry->d_name);  // Convert directory name to an integer (PID)
            if (pid > max_pid) {
                max_pid = pid;  // Keep track of the highest PID
            }
        }
    }
    closedir(proc_dir);
    return max_pid;
}

// Non-blocking input check
int check_key_press() {
    struct timeval tv = {0, 0};
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    return select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv);
}

// Enable non-canonical mode
void enable_non_canonical_mode() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~(ICANON | ECHO);  // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

// Restore the original terminal settings
void disable_non_canonical_mode() {
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag |= (ICANON | ECHO);  // Enable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

// The main neonate function
void neonate_main(int time_arg) {
    printf("Press 'x' to terminate\n");

    // Enable non-canonical mode
    enable_non_canonical_mode();

    while (1) {
        int pid = get_recent_pid();
        if (pid != -1) {
            printf("%d\n", pid);  // Print the most recent PID
        }

        // Sleep for the specified time_arg seconds
        sleep(time_arg);

        // Check if 'x' is pressed to terminate
        if (check_key_press()) {
            char ch = getchar();
            if (ch == 'x') {
                printf("Terminating...\n");
                break;
            }
        }
    }

    // Restore the terminal settings before exiting
    disable_non_canonical_mode();
}
