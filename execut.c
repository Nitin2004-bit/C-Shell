#include"header.h"

void hop(char* command); // Function declaration
void reveal(char* command); // Function declaration
void logs(char * command);

void exect(char* command){
    // if(strstr(command,"|")){ //for pipes
                
    //             flag_pipe=1;
    //         }
    //         if(flag_pipe==1){
    //             add_log(command,"");
    //             pipe_handler(command);
    //             flag_pipe=0;
    //             return;
    //         }
    //         if(strstr(command,">")||strstr(command,"<")||strstr(command,">>")){ // for redirection
    //             redir=1;
    //             // printf("in\n");
    //         }
    //         if(redir==1){
    //             add_log(command,"");
    //             io_handler(command);
                
    //             redir=0;
    //             return;
    //         }

    // printf("%s\n",command);
    char* tok = strtok(command, " \n");
            // printf("tok=%s1\n",tok);
            if (tok) {
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
                } else if (!strcmp(tok, "reveal")) {
                    // printf("in2\n");
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
                        add_log(tok,command);
                    }else{
                        // printf("in2\n");
                        strncpy(command, token, sizeof(command));
                        
                    }
                    
                    
                    
                    command[sizeof(command) - 1] = '\0'; // Ensure null termination
                    add_log(tok,command);
                    // printf("Debug: Calling proclore with command: %s\n", command); // Debug print
                    proclore(command);
                }else if (!strcmp(tok, ";")) {
                    // Handle other commands or separators
                }else if(!strcmp(tok,"seek")){
                    char command[MY_MAX_INPUT];
                    char* token=strtok(NULL,"\n");
                    if(token==NULL){
                        perror("no arguements");
                        return;
                    }else{
                        strncpy(command,token,sizeof(command));
                    }
                    command[sizeof(command)-1]='\0';
                    add_log(tok,command);
                    // printf("Debug: Calling seek with command: %s\n", command); // Debug print
                    seek(command);
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
                }else{
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
}