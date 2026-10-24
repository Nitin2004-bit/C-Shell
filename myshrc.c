#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//assumption:- not " " in commands and functions havent been implemented yet

// Struct for storing aliases
typedef struct {
    char alias[MY_MAX_INPUT];
    char command[MY_MAX_INPUT];
} Alias;

// Struct for storing functions
typedef struct {
    char name[MY_MAX_INPUT];
    char commands[MY_MAX_INPUT];  // Function body
} Func;

Func functions[MY_MAX_INPUT];  // Array of functions
int function_count = 0;        // Track number of functions

Alias aliases[MY_MAX_INPUT];   // Array of aliases
int alias_count = 0;           // Track number of aliases

// Helper function to check if a line is empty or a comment
int is_comment_or_empty(const char* line) {
    // Skip leading whitespace
    while (*line == ' ' || *line == '\t') {
        line++;
    }
    // Return true if the line is empty or starts with a '#'
    return (*line == '#' || *line == '\0');
}

// Helper function to strip comments (anything after '#') from a line
void strip_comments(char* line) {
    char* comment_start = strchr(line, '#');
    if (comment_start) {
        *comment_start = '\0';  // Terminate the string at the start of the comment
    }
}

// Function to load .myshrc and store aliases and functions
void load_myshrc() {
    FILE *file = fopen(".myshrc", "r");
    if (!file) {
        perror("Failed to open .myshrc");
        return;
    }

    char line[MY_MAX_INPUT];
    while (fgets(line, sizeof(line), file)) {
        // Strip comments from the line first
        strip_comments(line);

        // Ignore comments and empty lines
        if (is_comment_or_empty(line)) {
            continue;
        }

        // Parse lines like "alias reveall = reveal -l"
        if (strstr(line, "alias")) {
            char *temp = strtok(line, " =");
            char *alias_name = strtok(NULL, " =");
            char *alias_value = strtok(NULL, "\n");
            if (alias_name && alias_value) {
                strncpy(aliases[alias_count].alias, alias_name, MY_MAX_INPUT);  // Store alias name
                strncpy(aliases[alias_count].command, alias_value, MY_MAX_INPUT);  // Store alias command
                alias_count++;
            }
        }
        // Parse function definitions like "mk_hop() { mkdir ... }"
        else if (strstr(line, "()")) {
            char *func_name = strtok(line, " ()");
            // printf("func_name=%s\n",func_name);
            if (func_name) {
                strncpy(functions[function_count].name, func_name, MY_MAX_INPUT);  // Store function name
                functions[function_count].commands[0] = '\0';  // Initialize function body

                // Read the body of the function
                while (fgets(line, sizeof(line), file)) {
                    // Strip comments inside function body
                    strip_comments(line);

                    // Ignore empty lines or lines that are comments
                    if (is_comment_or_empty(line)) {
                        continue;
                    }

                    // Stop reading when we hit the closing '}'
                    if (strchr(line, '}')) {
                        break;
                    }

                    // Append the current line to the function's body
                    strncat(functions[function_count].commands, line, MY_MAX_INPUT - strlen(functions[function_count].commands) - 1);
                }
                // printf("func_command = %s %s %d\n",functions[function_count].name,functions[function_count].commands,function_count);
                function_count++;
            }
        }
    }

    fclose(file);
}

// Function to check if a command matches an alias
const char* check_alias(const char* command) {
    for (int i = 0; i < alias_count; i++) {
        if (strcmp(aliases[i].alias, command) == 0) {
            return aliases[i].command;  // Return the alias command if matched
        }
    }
    return command;  // Return original command if no alias is found
}
