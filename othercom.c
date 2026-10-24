#include"header.h"

#define MAX_ARGS 100
int bg=0;
struct Backgroundprocess bgp[100]={0};

void signalhandler(int sig) {
    int status;
    pid_t rs = waitpid(-1, &status, WNOHANG);

    if (rs > 0) {
        // Loop through the bgp array to find the matching pid
        for (int i = 0; i < 100; i++) {
            if (bgp[i].pid == rs) {  // Match found
                if (WIFEXITED(status)) {
                    printf("%s exited normally (%d)\n", bgp[i].name, bgp[i].pid);
                } else if (WIFSIGNALED(status)) {
                    printf("%s exited abnormally (%d)\n", bgp[i].name, bgp[i].pid);
                }
                
                // Mark the slot as empty
                bgp[i].pid = 0;
                bgp[i].name[0] = '\0';  // Clear the name string

                break;  // Exit loop once the matching process is found
            }
        }
    } else if (rs == 0) {
        // The process is still running, no need to do anything
        return;
    } else {
        // Only print an error message if waitpid returns -1
        // perror("Error in waitpid for background process");
    }
}


void execute_command(char* command) {
    signal(SIGCHLD, signalhandler);
    // signal(SIGTSTP,hand_sig);
    pid_t rd = fork();
        if (rd == 0) {
            // fd_pid=getpid();
        // signal(SIGTSTP, hand_sig);
        // Child process
        char* argv[MAX_ARGS];
        char* token;
        int i = 0;

        token = strtok(command, " ");
        while (token != NULL && i < MAX_ARGS - 1) {
            
            if (token[0] == '\'') {
                // If the token starts with a single quote, handle the entire quoted string
                argv[i] = malloc(MY_MAX_INPUT); // Allocate space for the full quoted string
                strcpy(argv[i], token + 1); // Copy the first part of the quoted string (skip the starting quote)

                // Continue reading tokens until the closing single quote is found
                while (token[strlen(token) - 1] != '\'' && (token = strtok(NULL, " \n")) != NULL) {
                    strcat(argv[i], " ");  // Add space between the tokens
                    strcat(argv[i], token); // Append the next part of the string
                }

                // Remove the closing single quote
                if (argv[i][strlen(argv[i]) - 1] == '\'') {
                    argv[i][strlen(argv[i]) - 1] = '\0';
                }

                i++; // Move to the next argument
            } else {
                // For normal tokens, store them directly
                argv[i++] = token;
            }
            token = strtok(NULL, " \n");
        }

        argv[i] = NULL;  // Null-terminate the argv array

        // Debugging: print the argv array
        // for (int j = 0; j < i; j++) {
        //     printf("%s %d\n", argv[j], j);
        // }

        // Execute the command

        execvp(argv[0], argv);

        // If execvp returns, there was an error
        perror("execvp failed");
        exit(EXIT_FAILURE);
    }else if (rd > 0) {
        if (!bg) {
            int status;
            struct timeval start, end;
            gettimeofday(&start, NULL);
            fd_pid=rd;
            
            waitpid(rd, &status, WUNTRACED);
            gettimeofday(&end, NULL);
            double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
            if (elapsed > 2.0) {
                printf("Foreground process '%s' took %d time to finish\n", command, (int)elapsed);
                // time_elapsed and cmd_el should be defined globally or passed as arguments
                time_elapsed = (int)elapsed;
                cmd_el = command;
            }
            fd_pid=-2;
            // printf("process successful\n");
        } else {
            printf("PID=%d\n", (int)rd);
            int count = -1;
            for (int i = 0; i < 100; i++) {
                if (bgp[i].pid == 0) {
                    count = i;
                    bgp[i].pid = rd;
                    strncpy(bgp[i].name, command, sizeof(bgp[i].name) - 1);
                    bgp[i].name[sizeof(bgp[i].name) - 1] = '\0';  // Ensure null-termination
                    break;
                }
            }
            if (count == -1) {
                printf("Max background processes reached\n");
            }
        }
    } else {
        perror("fork failed");
    }
}


// void check_bg() {
//     for (int i = 0; i < 100; i++) {
//         if (bgp[i].pid == 0) {
//             // Skip empty slots
//             continue;
//         } else {
//             int status;
//             pid_t rs = waitpid(bgp[i].pid, &status, WNOHANG);
//             if (rs > 0) {
//                 if (WIFEXITED(status)) {
//                     printf("%s exited normally (%d)\n", bgp[i].name, bgp[i].pid);
//                     bgp[i].pid = 0; // Mark the slot as empty
//                 } else if (WIFSIGNALED(status)) {
//                     printf("%s exited abnormally (%d)\n", bgp[i].name, bgp[i].pid);
//                     bgp[i].pid = 0; // Mark the slot as empty
//                 }
//             } else if (rs == 0) {
//                 // The process is still running, no need to do anything
//                 continue;
//             } else {
//                 // Only print an error message if waitpid returns -1
//                 perror("Error in waitpid for background process");
//             }
//         }
//     }
// }


void others(char* command){
    // printf("com=%s\n",command);
    bg=0;
    if(strstr(command,"&")){
        bg=1;
        command[strcspn(command,"&")]=0; //remove &
        
    }
    // printf("in1\n");
    // if(strstr(command,"ls")){
    //     char* tok=strtok(command," ");
    //     while(tok!=NULL ){
    //         printf("tok=%s\n",tok);
    //         snprintf(command,sizeof(command),"%s ",tok);
    //         tok=strtok(NULL," ");
    //     }
    // }
    // char* p=NULL;
    // printf("%s\n",p);
    // printf("commm=%s\n",command);
    
    // printf("%s\n",command);
    execute_command(command);
}