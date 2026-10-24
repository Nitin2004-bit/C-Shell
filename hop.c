#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "header.h"

char hop_file[MY_MAX_INPUT];
char original[MY_MAX_INPUT];

void initialize_hop() {
    if (getcwd(original, sizeof(original)) == NULL) {
        perror("Failed to get current working directory");
        exit(1);  // Exit if initialization fails
    }

    if (getcwd(hop_file, sizeof(hop_file)) != NULL) {
        // Append "/hop.txt" to the path
        strncat(hop_file, "/hop.txt", sizeof(hop_file) - strlen(hop_file) - 1);
    } else {
        perror("Failed to get current working directory");
        exit(1);  // Exit if initialization fails
    }
}

void update_hop_file(const char* dir) {
    FILE* fx = fopen(hop_file, "w");
    if (!fx) {
        perror("Failed to open hop.txt for writing");
        return;
    }

    fprintf(fx, "%s\n", dir);
    fclose(fx);
}

void hop(char* command) {
    FILE* fx = fopen(hop_file, "r+");
    if (!fx) {
        perror("Failed to open file");
        return;
    }
    
    char previous_dir[MY_MAX_INPUT];
    char current_dir[MY_MAX_INPUT];
    
    if (getcwd(current_dir, sizeof(current_dir)) == NULL) {
        perror("getcwd() error");
    }

    char t[MY_MAX_INPUT];
    strncpy(t, command, sizeof(t));
    t[sizeof(t) - 1] = '\0';
    
    char* tok;
    tok = strtok(t, " /");
    while (tok != NULL) {
        if (!strcmp(tok, ".")) {
            if (getcwd(current_dir, sizeof(current_dir)) == NULL) {
                perror("getcwd error");
            } else {
                fprintf(stdout, "%s\n", current_dir);
            }
        } else if (!strcmp(tok, "..")) {
            if (getcwd(previous_dir, sizeof(previous_dir)) == NULL) {
                perror("getcwd error");
            } else {
                update_hop_file(previous_dir); // Update the file with the previous directory
                chdir("..");

                if (getcwd(current_dir, sizeof(current_dir)) == NULL) {
                    perror("getcwd error");
                } else {
                    fprintf(stdout, "%s\n", current_dir);
                }
            }
        } else if (!strcmp(tok, "~")) {
            if (getcwd(previous_dir, sizeof(previous_dir)) == NULL) {
                perror("getcwd error");
            }else{
                update_hop_file(previous_dir);
                if (chdir(original) != 0) {
                    perror("chdir error");
                }

                if (getcwd(current_dir, sizeof(current_dir)) == NULL) {
                    perror("getcwd error");
                } else {
                    fprintf(stdout, "%s\n", current_dir);
                }
            }
            // Change to the original directory (where the program was started)
        } else if (!strcmp(tok, "-")) {
            char q[MY_MAX_INPUT];
            while (fgets(q, sizeof(q), fx)) {
                break;
            }
            fseek(fx, 0, SEEK_SET);
            ftruncate(fileno(fx), 0);
            getcwd(previous_dir, sizeof(previous_dir));
            fprintf(fx, "%s", previous_dir);
            
            q[strcspn(q, "\n")] = 0; // Remove trailing newline
            chdir(q);

            if (getcwd(current_dir, sizeof(current_dir)) == NULL) {
                perror("getcwd error");
            } else {
                fprintf(stdout, "%s\n", current_dir);
            }
        } else {
            if (!strcmp(tok, "hop") || !strcmp(tok, "\n")) {
                // Do nothing
            } else {
                char temp[MY_MAX_INPUT];
                char temp2[MY_MAX_INPUT];
                getcwd(temp, sizeof(temp));
                snprintf(temp2, sizeof(temp2), "%s/%s", temp, tok);
                struct stat filestat;
                if (stat(temp2, &filestat) < 0) {
                    perror("stat error");
                }
                if (S_ISDIR(filestat.st_mode)) {
                    getcwd(previous_dir, sizeof(previous_dir));
                    update_hop_file(previous_dir); // Update the file with the previous directory
                    chdir(temp2);
                    if (getcwd(current_dir, sizeof(current_dir)) == NULL) {
                        perror("getcwd error");
                    } else {
                        fprintf(stdout, "%s\n", current_dir);
                    }
                } else {
                    perror("directory not available");
                }
            }
        }
       
        tok = strtok(NULL, " /");
    }

    fclose(fx);
}
