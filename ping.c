#define _POSIX_C_SOURCE 200809L
#include"header.h"
// #include <sigset-cvt-mask.h>
#include<errno.h>

bool check_process(int procid){
    for(int i=0;i<100;i++){
        if(bgp[i].pid==procid){
            return true;
        }
    }
    return false;
}

void send_signal(int procid,int signal){
    int sig=signal % 32;
    if (kill(procid, signal) == -1) {
        if (errno == ESRCH) {
            printf("No such process found\n");
        } else {
            perror("Error sending signal");
        }
    } else {
        printf("Sent signal %d to process with pid %d\n", signal, procid);
    }
}


// // void handle_sigtstp(int signo) {
//     if (fd_pid > 0) {
//         printf("\nStopping foreground process %d\n", fd_pid);
//         kill(fd_pid, SIGTSTP);  // Send SIGTSTP to the foreground process

//         // Add the stopped process to the background process list
//         for (int i = 0; i < 100; i++) {
//             if (bgp[i].pid == 0) {
//                 bgp[i].pid = fd_pid;
//                 strcpy(bgp[i].name, "Stopped Process");
//                 break;
//             }
//         }

//         fd_pid = -1; // Reset fd_pid as the process is no longer in foreground
//     } else {
//         printf("\nNo foreground process to stop\n");
//     }

//     // Unblock SIGTSTP for this process
//     sigset_t set;
//     sigemptyset(&set);
//     sigaddset(&set, SIGTSTP);
//     sigprocmask(SIG_UNBLOCK, &set, NULL);
// }
void PrintSignalMask() {
    sigset_t current_mask;
    sigprocmask(SIG_BLOCK, NULL, &current_mask);

    printf("Current signal mask: ");
    if (sigismember(&current_mask, SIGINT)) {
        printf("SIGINT ");
    }
    if (sigismember(&current_mask, SIGQUIT)) {
        printf("SIGQUIT ");
    }
    if (sigismember(&current_mask, SIGTSTP)) {
        printf("SIGTSTP ");
    }
    printf("\n");
}
void Unblocksignals() {
    sigset_t block_mask, oldmask;
    // Retrieve the current signal mask
    sigprocmask(SIG_BLOCK, NULL, &oldmask); 
    PrintSignalMask();

    // Initialize the signal set
    sigemptyset(&block_mask);

    // Add the signals SIGINT and SIGTSTP to the set
    sigaddset(&block_mask, SIGINT);
    sigaddset(&block_mask,SIGQUIT);
    sigaddset(&block_mask, SIGTSTP);

    // Unblock the signals
    if (sigprocmask(SIG_UNBLOCK, &block_mask, NULL) == -1) {
        perror("sigprocmask");
    }
    PrintSignalMask();
}
void hand_sig(int signal){
    switch(signal){
        case SIGINT: // ctrl c
            // Handling for SIGINT remains unchanged
            printf("pid=%d\n",fd_pid);
            if(fd_pid>0){
                kill(fd_pid,SIGKILL);
                // Unblocksignals();
                fd_pid=-2;
            }else{
                perror("no foreground process");
                // Unblocksignals();
            }
            
            // return;
            break;
        case SIGTSTP: // ctrl z
            printf("pid=%d\n",fd_pid);
            if(fd_pid > 0){
                printf("\nStopping foreground process %d\n", fd_pid);
                kill(fd_pid, SIGTSTP);
                // Add the stopped process to your background processes list if needed
                for(int i=0; i<100; i++){
                    if(bgp[i].pid == 0){
                        bgp[i].pid = fd_pid;
                        strcpy(bgp[i].name, "Stopped Process"); // You might want to store the actual command name
                        break;
                    }
                }
                // Unblocksignals();
                fd_pid = -2; // Reset fd_pid as the process is no longer in foreground
            } else {
                printf("\nNo foreground process to stop\n");
                // Unblocksignals();
            }   
                // printf("%d\n",(int)getpid());
                // kill(getpid(),SIGCONT);
                // printf("in1\n");
                z_flag=1;
                // main();
                
            // Unblocksignals();
            break;
        case SIGQUIT: // ctrl d
            // Handling for SIGQUIT remains unchanged
            break;
    }
}

void ping(char* command){
    // The ping function remains unchanged
    char* tok=strtok(command," \n");
    int procid=atoi(tok);
    if(check_process(procid)==0){
        perror("no such process");
        return;
    }
    tok=strtok(NULL," \n");
    if(tok==NULL){
        perror("no signal");
        return;
    }
    int signal=atoi(tok);
    send_signal(procid,signal);
}