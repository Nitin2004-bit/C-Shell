#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include"header.h"

int found = 0;
int file_count = 0;
int dir_count = 0;
char last_file[MY_MAX_INPUT];
char last_dir[MY_MAX_INPUT];
char originalpath[MY_MAX_INPUT];
void print_relative_path(char* originalpath, char* fullpath) {
    // Find the common prefix length
    int i = 0;
    while (originalpath[i] != '\0' && fullpath[i] != '\0' && originalpath[i] == fullpath[i]) {
        i++;
    }

    // Skip any remaining slashes in the original path
    while (originalpath[i] == '/') {
        i++;
    }

    // Print the relative path starting from where the paths differ
    printf("%s\n", fullpath + i);
}


void seek_info(int file_flag, int dir_flag, int e_flag, char* name, char* path) {
    DIR* dir = opendir(path);
    struct dirent* entry;
    struct stat entry_stat;

    if (dir == NULL) {
        perror("opendir failed");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        char fullpath[MY_MAX_INPUT];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

        if (stat(fullpath, &entry_stat) == -1) {
            continue;
        }

        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char *filename = entry->d_name;
        char name_no_ext[MY_MAX_INPUT];
        strncpy(name_no_ext, filename, sizeof(name_no_ext) - 1);
        name_no_ext[sizeof(name_no_ext) - 1] = '\0';

        char *ext = strrchr(name_no_ext, '.');
        if (ext != NULL) {
            *ext = '\0';  // Remove the extension
        }

        if (!strcmp(name_no_ext, name)) {
            if (file_flag && S_ISREG(entry_stat.st_mode)) {
                found = 1;
                file_count++;
                strncpy(last_file, fullpath, sizeof(last_file) - 1);
                last_file[sizeof(last_file) - 1] = '\0';
                printf("\033[0;32m");
                print_relative_path(originalpath, fullpath);
                printf("\033[0m");
            } else if (dir_flag && S_ISDIR(entry_stat.st_mode)) {
                found = 1;
                dir_count++;
                strncpy(last_dir, fullpath, sizeof(last_dir) - 1);
                last_dir[sizeof(last_dir) - 1] = '\0';
                printf("\033[0;34m");
                print_relative_path(originalpath, fullpath);
                printf("/\033[0m");
            } else if (!file_flag && !dir_flag) {
                found = 1;
                if (S_ISREG(entry_stat.st_mode)) {
                    file_count++;
                    strncpy(last_file, fullpath, sizeof(last_file) - 1);
                    last_file[sizeof(last_file) - 1] = '\0';
                    printf("\033[0;32m");
                    print_relative_path(originalpath, fullpath);
                    printf("\033[0m");
                } else if (S_ISDIR(entry_stat.st_mode)) {
                    dir_count++;
                    strncpy(last_dir, fullpath, sizeof(last_dir) - 1);
                    last_dir[sizeof(last_dir) - 1] = '\0';
                    printf("\033[0;34m");
                    print_relative_path(originalpath, fullpath);
                    printf("/\033[0m");
                }
            }
        }

        if (S_ISDIR(entry_stat.st_mode)) {
            seek_info(file_flag, dir_flag, e_flag, name, fullpath);
        }
    }

    closedir(dir);
}


void seek(char* command) {
    int file_flag = 0;
    int dir_flag = 0;
    int e_flag = 0;
    char* path = NULL;
    char* name;
    char* tok = strtok(command, " ");

    while (tok != NULL) {
        
        // printf("tok=%s\n",tok);
        if (strstr(tok, "-f")) {
            file_flag = 1;
        } else if (strstr(tok, "-d")) {
            dir_flag = 1;
        } else if (strstr(tok, "-e")) {
            e_flag = 1;
        } else if (strstr(tok, "./") || tok[0] == '.' || tok[0] == '/') {
            path = tok;
        } else {
            name = tok;
        }
        if (file_flag == 1 && dir_flag == 1) {
            perror("invalid flags");
            return;
        }

        tok = strtok(NULL, " ");
    }
    // printf("%d %d\n",file_flag,dir_flag);
    if (path == NULL) {
        char cwd[MY_MAX_INPUT];
        getcwd(cwd, sizeof(cwd));
        path = cwd;
    }

    if (name == NULL) {
        perror("file name not provided");
        return;
    }
    strcpy(originalpath,path);

    seek_info(file_flag, dir_flag, e_flag, name, path);

    // Handle the `-e` flag
    if (e_flag) {
        if (file_count == 1 && dir_count == 0) {
            FILE* fp = fopen(last_file, "r");
            if (fp == NULL) {
                printf("Missing permissions for task!\n");
            } else {
                char line[MY_MAX_INPUT];
                while (fgets(line, sizeof(line), fp) != NULL) {
                    printf("%s", line);
                }
                fclose(fp);
            }
        } else if (dir_count == 1 && file_count == 0) {
            if (chdir(last_dir) == -1) {
                printf("Missing permissions for task!\n");
            } else {
                printf("Changed directory to %s\n", last_dir);
            }
        }
    }

    if (!found) {
        printf("No match found!\n");
    }
}


