#include <stdio.h>
#include <string.h>
#include "header.h"

void input(char* inputfile) {
    char input[MY_MAX_INPUT];
    fgets(input, MY_MAX_INPUT, stdin);
    // if(input==NULL){
    //     if(feof(stdin)){
    //         printf("in\n");
    //     }
    // }


    char* delim = "\n\t ";
    char* tok;
    FILE* fd = fopen(inputfile, "w");
    if (fd == NULL) {
        printf("Failed to open file.\n");
        return;
    }

    tok = strtok(input, delim);
    while (tok != NULL) {
        // Check if the token is an alias and replace it
        const char* actual_command = check_alias(tok);
        
        // Create a buffer to hold the command that can be modified
        char command_buffer[MY_MAX_INPUT];
        strncpy(command_buffer, actual_command, sizeof(command_buffer));
        command_buffer[sizeof(command_buffer) - 1] = '\0';  // Ensure null termination
        // if(strstr(command_buffer,"|")){
        //     flag_pipe=1;
        // }
        // Check for special characters and handle them
        if (strcmp(command_buffer, ";") == 0 || strcmp(command_buffer, "&") == 0) {
            if (!strcmp(command_buffer, "&")) {
                fprintf(fd, "%s\n", command_buffer);
            } else {
                fprintf(fd, "\n%s\n", command_buffer);
            }
            log_flag = 1;
        } else if (!strcmp(command_buffer, " ")) {
            // Ignore spaces
        } else {
            if (!strcmp(command_buffer, "log")) {
                log2_flag = 1;
                fprintf(fd, "%s ", command_buffer);

                char* next_token = strtok(NULL, "\n");
                if (next_token != NULL) {
                    fprintf(fd, "%s", next_token);
                }
                break;
            } else if (strstr(command_buffer, ";")) {
                command_buffer[strcspn(command_buffer, ";")] = '\0';  // Null-terminate before the semicolon
                fprintf(fd, "%s\n%s\n", command_buffer, ";");
            } else if (strstr(command_buffer, "&")) {
                command_buffer[strcspn(command_buffer, "&")] = '\0';  // Null-terminate before the ampersand
                fprintf(fd, "%s %s\n", command_buffer, "&");
            } else {
                fprintf(fd, "%s ", command_buffer);
            }
        }
        tok = strtok(NULL, delim);
    }

    fprintf(fd, "\n"); // To ensure a newline at the end of the line
    fclose(fd);
}
