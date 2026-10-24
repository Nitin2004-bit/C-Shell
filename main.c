#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include<signal.h>
#include "header.h"

void hop(char* command); // Function declaration
void reveal(char* command); // Function declaration
void logs(char * command);
int log_flag=0;
int log2_flag=0;
int time_elapsed;
char* cmd_el;
int redir=0;
int flag_pipe=0;
int end_flag=0;
int fd_pid=-2;
int z_flag=0;
// void print_signal_mask() {
//     sigset_t current_mask;
//     sigprocmask(SIG_SETMASK, NULL, &current_mask);
//     if (sigismember(&current_mask, SIGINT)) {
//         printf("SIGINT is blocked\n");
//     } else {
//         printf("SIGINT is not blocked\n");
//     }
// }


// volatile sig_atomic_t received_sigtstp = 0;

// void handle_sigtstp(int signo) {
//     received_sigtstp = 1;
// }

// void process_sigtstp() {
//     if (received_sigtstp && fd_pid > 0) {
//         printf("\nStopping foreground process %d\n", fd_pid);
//         kill(fd_pid, SIGTSTP);
        
//         // Add the stopped process to the background process list
//         for (int i = 0; i < 100; i++) {
//             if (bgp[i].pid == 0) {
//                 bgp[i].pid = fd_pid;
//                 strcpy(bgp[i].name, "Stopped Process");
//                 break;
//             }
//         }
        
//         fd_pid = -1;
//         received_sigtstp = 0;
//     }
// }


void hand_stgint(int signal){
    if(fd_pid>0){
        // printf("%d\n",fd_pid);
        kill(fd_pid,SIGKILL);
        fd_pid=-2;
    }else{
        perror("no foreground process");
    }
    // Unblocksignals();
    // print_signal_mask();
}

void hand_stgstp(int signal){
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
             
}
int main() {
    
    // printf("%s\n",orginal);



    // signal(SIGTSTP,hand_sig);

    // struct sigaction sa;
    // sa.sa_handler = handle_sigtstp;
    // sa.sa_flags = ERESTART;
    // sigemptyset(&sa.sa_mask);
    
    // if (sigaction(SIGTSTP, &sa, NULL) == -1) {
    //     perror("Error setting up SIGTSTP handler");
    //     exit(1);
    // }
    // sigset_t block_mask, oldmask;
    // sigemptyset(&block_mask);           // Initialize the signal set
    // sigaddset(&block_mask, SIGTSTP);    // Add SIGTSTP to the set
    // sigaddset(&block_mask,SIGINT);
    // sigprocmask(SIG_BLOCK, &block_mask, &oldmask);
    
    initialize_logs();
    initialize_hop();
    initialize_reveal();

    load_myshrc();
    char input_file[MY_MAX_INPUT];

    // signal(SIGINT,hand_stgint);

    struct sigaction sa;
    sa.sa_handler = hand_stgint;
    sa.sa_flags = 0;  // Can set SA_RESTART if needed
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("Error setting SIGINT handler");
        exit(EXIT_FAILURE);
    }
    // signal(SIGTSTP,hand_sig);
    struct sigaction sa_stp;
    sa_stp.sa_handler = hand_stgstp;
    sa_stp.sa_flags = 0;  // Can set SA_RESTART if needed
    sigemptyset(&sa_stp.sa_mask);
    if (sigaction(SIGTSTP, &sa_stp, NULL) == -1) {
        perror("Error setting SIGTSTP handler");
        exit(EXIT_FAILURE);
    }
    signal(SIGQUIT,hand_sig);
    
    // Get the current working directory
    if (getcwd(input_file, sizeof(input_file)) != NULL) {
        // Append "/inputs.txt" to the path
        strncat(input_file, "/inputs.txt", sizeof(input_file) - strlen(input_file) - 1);
    } else {
        perror("Failed to get current working directory");
        return 1;
    }
    // Unblocksignals();
    
    
    while (1) {
        // Placeholder for basic() and input() functions
        // Unblocksignals();
        // printf("%d\n",getpid());
        log_flag=0;
        flag_pipe=0;
        if(feof(stdin)){
            printf("\n");
            break;
        }
        
        basic();
        input(input_file);

        if(end_flag==1){
            break;
        }
        // signal(SIGQUIT,hand_sig);
        // break;
        // check_bg();

        FILE *fd = fopen(input_file, "r");
        if (!fd) {
            perror("Failed to open file");
            continue;
        }
        
        char t[MY_MAX_INPUT];
        while (fgets(t, sizeof(t), fd)) {
            // pid_t rs=fork();

            // if(rs==0){
            if (feof(fd)) {
                end_flag = 1;
            }
                if(strstr(t,"|")){ //for pipes
                    
                    flag_pipe=1;
                }
                if(flag_pipe==1){
                    add_log(t,"");
                    pipe_handler(t);
                    flag_pipe=0;
                    continue;
                }
                if(strstr(t,">")||strstr(t,"<")||strstr(t,">>")){ // for redirection
                    redir=1;
                    // printf("in\n");
                }
                if(redir==1){
                    add_log(t,"");
                    io_handler(t);
                    
                    redir=0;
                    continue;
                }
                
                
                char* tok = strtok(t, " \n");
                // printf("tok=%s\n",tok);
                if (tok) {
                    // const char* actual_command=check_alias(tok);
                    if (!strcmp(tok, "hop")) {
                    char command[MY_MAX_INPUT];
                        char* cmd_part = strtok(NULL, "\n");
                        
                        if (cmd_part == NULL) {
                            strncpy(command, ".", sizeof(command)); // Set command to "." if NULL
                        } else {
                            strncpy(command, cmd_part, sizeof(command));
                        }

                        command[sizeof(command) - 1] = '\0'; // Ensure null termination
                        // printf("Debug: Calling hop with command: %s\n", command); // Debug print
                        // printf("%s\n",tok);
                        add_log(tok,command);
                        hop(command);
                    }else if (!strcmp(tok, "reveal")) {
                        char command[MY_MAX_INPUT];
                        char* cmd_part = strtok(NULL, "\n");
                        
                        if (cmd_part == NULL) {
                            strncpy(command, ".", sizeof(command)); // Set command to "." if NULL
                        } else {
                            strncpy(command, cmd_part, sizeof(command));
                        }

                        command[sizeof(command) - 1] = '\0'; // Ensure null termination

                        // Debug print if needed
                        // printf("Debug: Calling reveal with command: %s\n", command);

                        add_log(tok, command);
                        reveal(command);
                    }else if(!strcmp(tok,"log")){
                        // printf("in\n");
                        char command[MY_MAX_INPUT];
                        char* token = strtok(NULL, "\n");  // Get the next token
                        if (token == NULL) {
                            strncpy(command,".",sizeof(command)-1);
                        }else{
                            // printf("in2\n");
                            strncpy(command, token, sizeof(command));
                        }
                        
                        
                        
                        command[sizeof(command) - 1] = '\0'; // Ensure null termination
                        // printf("Debug: Calling logs with command: %s\n", command); // Debug print
                        logs(command);
                    }else if(!strcmp(tok,"proclore")){
                        char command[MY_MAX_INPUT];
                        char* token = strtok(NULL, "\n");  // Get the next token
                        if (token == NULL) {
                            strncpy(command,".",sizeof(command)-1);
                        }else{
                            // printf("in2\n");
                            strncpy(command, token, sizeof(command));
                        }
                        
                        
                        
                        command[sizeof(command) - 1] = '\0'; // Ensure null termination
                        // printf("Debug: Calling proclore with command: %s\n", command); // Debug print
                        proclore(command);
                    }else if(!strcmp(tok,"seek")){
                        char command[MY_MAX_INPUT];
                        char* token=strtok(NULL,"\n");
                        if(token==NULL){
                            perror("no arguements");
                            continue;
                        }else{
                            strncpy(command,token,sizeof(command));
                        }
                        command[sizeof(command)-1]='\0';
                        // printf("Debug: Calling seek with command: %s\n", command); // Debug print
                        seek(command);
                    }else if (!strcmp(tok, ";")) {
                        // Handle other commands or separators
                    }else if(!strcmp(tok,"activities")){
                        char* token=strtok(NULL," \n");
                        if(token==NULL){
                            add_log(tok,"");
                            activity();
                        }else{
                            perror("no arguements expected for activities");
                        }


                    }else if(!strcmp(tok,"ping")){
                        char* token=strtok(NULL,"\n");
                        if(token==NULL){
                            perror("no arguements");

                        }else{
                            add_log(tok,token);
                            ping(token);
                        }
                    }else if(!strcmp(tok,"fg")){
                        char* token =strtok(NULL," \n");
                        char cmd_input[MY_MAX_INPUT];
                        if(token==NULL){
                            perror("no arguements");
                        }else{
                            int fk_pid=atoi(token);
                            snprintf(cmd_input,MY_MAX_INPUT,"%s",token);
                            add_log(tok,cmd_input);
                            fg(fk_pid);
                        }
                    }else if(!strcmp(tok,"bg")){
                        char* token =strtok(NULL," \n");
                        char cmd_input[MY_MAX_INPUT];
                        if(token==NULL){
                            perror("no arguements");
                        }else{
                            int bk_pid=atoi(token);
                            snprintf(cmd_input,MY_MAX_INPUT,"%s",token);
                            add_log(tok,cmd_input);
                            bgs(bk_pid);
                        }
                    }else if(!strcmp(tok,"iMan")){
                         char* token=strtok(NULL," \n");
                         if(token==NULL){
                            perror("no arguements");

                         }else{
                            add_log(tok,token);
                            Iman(token);
                         }
                    }else if(!strcmp(tok,"neonate")){
                        char* token=strtok(NULL," \n");
                        if(token==NULL){
                            perror("no arguements");
                        }else{
                            if(!strstr(token,"-n")){
                                perror("no flag");
                            }else{
                                char* cmd_snd;
                                cmd_snd=strtok(NULL,"\n");
                                char cmd_end[MAX_INPUT];
                                snprintf(cmd_end,MAX_INPUT,"%s %s",token,cmd_snd);
                                add_log(tok,cmd_end); // Logging the command
                                
                                int time_arg = atoi(cmd_snd);  // Convert argument to an integer
                                if (time_arg > 0) {
                                    neonate_main(time_arg);  // Call the neonate function with the time interval
                                } else {
                                    perror("invalid time argument");
                                }

                            }

                        }

                    }
                    else{
                        char command[MY_MAX_INPUT];
                        char* token = strtok(NULL,"\n");
                        
                        // printf("%s\n",token);
                        if(token==NULL){
                            snprintf(command,sizeof(command),"%s",tok);
                            add_log(tok,"");
                        }else{

                        snprintf(command,sizeof(command),"%s %s",tok,token);
                        add_log(tok,token);
                        }
                        // char* t=strtok(command," ");
                        // while(t!=NULL){
                        //     printf("tok2=%s\n",t);
                        //     t=strtok(NULL,"");
                        // }
                        
                        others(command);
                        
                    }
                    
                    
                }

            // }else if(rs>0){ // end of if for child process
            //     wait(NULL);

            // }else{
            //     perror("fork error");
            // }
            if(end_flag==1){
                break;
            }
        }
        redir=0;

        fclose(fd);
        // process_sigtstp();
        // if(end_flag==1){
        //     break;
        // }
        // Unblocksignals();
    }
    // main();
        // printf("in\n");
}
