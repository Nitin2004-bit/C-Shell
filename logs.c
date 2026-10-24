#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  // For getcwd
#include "execut.h"
#include"header.h"

#define MAX_LOGS 15


typedef struct LinkedList {
    int count;
    char* log[MAX_LOGS];
} linkedlist;

linkedlist* lg;
char LOG_FILE[MY_MAX_INPUT];

void initialize_logs() {
    // Get the current working directory
    if (getcwd(LOG_FILE, sizeof(LOG_FILE)) != NULL) {
        // Append /logs.txt to the current working directory
        strncat(LOG_FILE, "/logs.txt", sizeof(LOG_FILE) - strlen(LOG_FILE) - 1);
    } else {
        perror("getcwd() error");
        return;
    }

    lg = malloc(sizeof(linkedlist));
    if (lg != NULL) {
        lg->count = 0;

        // Load logs from file
        FILE *file = fopen(LOG_FILE, "r");
        if (file != NULL) {
            char buffer[MY_MAX_INPUT];
            while (fgets(buffer, sizeof(buffer), file)) {
                buffer[strcspn(buffer, "\n")] = 0; // Remove newline character
                if (lg->count < MAX_LOGS) {
                    lg->log[lg->count] = strdup(buffer);
                    lg->count++;
                }
            }
            fclose(file);
        }
    }
}

void save_logs_to_file() {
    FILE *file = fopen(LOG_FILE, "w");
    if (file != NULL) {
        for (int i = 0; i < lg->count; i++) {
            fprintf(file, "%s\n", lg->log[i]);
        }
        fclose(file);
    } else {
        perror("Could not open log file for writing");
    }
}

void add_log(char* sd, char* command) {
    // printf("sd=%s cmd=%s\n",sd,command);
    char lm[MY_MAX_INPUT];
    // printf("cmd=%s1\n",command);
    if(!strcmp(command,"")){
        char* newline=strchr(sd,'\n');
        if(newline){
            *newline='\0';
        }
        snprintf(lm,sizeof(lm),"%s",sd);
    }else{
        snprintf(lm, sizeof(lm), "%s %s", sd, command);

    }
    // printf("lm=%s\n",lm);
    if(lg->count != 0) {
        if(!strcmp(lm, lg->log[lg->count-1])) {
            return;
        }
    }
    if(log_flag == 1 && log2_flag == 1) {
        return;
    }
    if (lg->count < MAX_LOGS) {
        lg->log[lg->count] = strdup(lm);
        lg->count++;
    } else {
        // Free the memory of the oldest log
        free(lg->log[0]);

        // Shift all logs to the left by one
        for (int i = 1; i < MAX_LOGS; i++) {
            lg->log[i - 1] = lg->log[i];
        }

        // Add the new log to the last index
        lg->log[MAX_LOGS - 1] = strdup(lm);
    }
    // printf("log_count=%d\n",lg->count);

    save_logs_to_file(); // Save the updated logs to the file
}

void print_log() {
    if (lg->count == 0) {
        perror("nothing in log");
    } else {
        for (int temp = lg->count - 1; temp >= 0; temp--) {
            printf("%s\n", lg->log[temp]);
        }
    }
}

void execute(int value) {
    if (value <= 0 || value > MAX_LOGS) {
        perror("Index error for execute log");
    } else {
        int idx = value - 1;
        char cmmd[MY_MAX_INPUT];

        if (lg->log[idx] == NULL) {
            perror("Log entry is NULL");
            return;
        }
        exect(lg->log[idx]);
    }
}

void logs(char* command) {
    char* t = strtok(command, " ");
    if (!strcmp(t, ".")) {
        print_log();
    } else if(!strcmp(t, "purge")) {
        lg->count = 0;
        for (int i = 0; i < MAX_LOGS; i++) {
            free(lg->log[i]);
            lg->log[i] = NULL;
        }

        if (remove(LOG_FILE) != 0) {
            perror("Error deleting log file");
        } else {
            printf("Log file deleted successfully.\n");
        }
    } else if(!strcmp(t, "execute")) {
        char* tok = strtok(NULL, " ");
        if(tok != NULL) {
            int value = atoi(tok);
            execute(value);
        } else {
            perror("no value");
        }
    }
}
