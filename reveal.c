#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include "header.h"
char hop_file2[MY_MAX_INPUT];
void print_file_info(const char *name, const char *path, bool long_list);

void initialize_reveal(){
    if (getcwd(hop_file2, sizeof(hop_file2)) != NULL) {
        // Append "/inputs.txt" to the path
        strncat(hop_file2, "/hop.txt", sizeof(hop_file2) - strlen(hop_file2) - 1);
    } else {
        perror("Failed to get current working directory");
        return;
    }
}
int compare_dirent(const struct dirent **a, const struct dirent **b) {
    return strcmp((*a)->d_name, (*b)->d_name);
}

void expand_path(char *path, char *expanded_path) {
    if (path[0] == '~') {
        const char *home = getenv("HOME");
        if (!home) {
            home = getpwuid(getuid())->pw_dir;
        }
        snprintf(expanded_path, MY_MAX_INPUT, "%s%s", home, path + 1);
    } else {
        strncpy(expanded_path, path, MY_MAX_INPUT);
    }
}

void list_directory(const char* path, bool show_all, bool long_list) {
    struct dirent **namelist;
    int n = scandir(path, &namelist, NULL, compare_dirent);
    if (n < 0) {
        perror("scandir error");
        return;
    }

    for (int i = 0; i < n; i++) {
        if (!show_all && namelist[i]->d_name[0] == '.') {
            free(namelist[i]);
            continue;
        }
        print_file_info(namelist[i]->d_name, path, long_list);
        free(namelist[i]);
    }
    free(namelist);
}

void print_file_info(const char *name, const char *path, bool long_list) {
    char fullpath[MY_MAX_INPUT];
    snprintf(fullpath, sizeof(fullpath), "%s/%s", path, name);

    struct stat file_stat;
    if (stat(fullpath, &file_stat) == -1) {
        perror("stat");
        return;
    }

    if (long_list) {
        printf((S_ISDIR(file_stat.st_mode)) ? "d" : "-");
        printf((file_stat.st_mode & S_IRUSR) ? "r" : "-");
        printf((file_stat.st_mode & S_IWUSR) ? "w" : "-");
        printf((file_stat.st_mode & S_IXUSR) ? "x" : "-");
        printf((file_stat.st_mode & S_IRGRP) ? "r" : "-");
        printf((file_stat.st_mode & S_IWGRP) ? "w" : "-");
        printf((file_stat.st_mode & S_IXGRP) ? "x" : "-");
        printf((file_stat.st_mode & S_IROTH) ? "r" : "-");
        printf((file_stat.st_mode & S_IWOTH) ? "w" : "-");
        printf((file_stat.st_mode & S_IXOTH) ? "x " : "- ");
        printf("%ld ", file_stat.st_nlink);
        printf("%s ", getpwuid(file_stat.st_uid)->pw_name);
        printf("%s ", getgrgid(file_stat.st_gid)->gr_name);
        printf("%5ld ", file_stat.st_size);
        printf("%.12s ", 4 + ctime(&file_stat.st_mtime));
    }

    // Color code for directory, executable, or regular file
    if (S_ISDIR(file_stat.st_mode)) {
        printf("\033[1;34m%s\033[0m\n", name);  // Blue for directories
    } else if (file_stat.st_mode & S_IXUSR) {
        printf("\033[1;32m%s\033[0m\n", name);  // Green for executables
    } else {
        printf("\033[0m%s\n", name);  // Default color for files
    }
}


void reveal(char* command) {
    char* flags = strtok(command, " ");
    char* path = strtok(NULL, " ");
    bool show_all = false;
    bool long_list = false;
    char expanded_path[MY_MAX_INPUT];

    // Handle default path if none is provided
    if (!path) {
        path = ".";
    }

    // Check if flags are present
    if (flags && flags[0] == '-') {
        if (strstr(flags, "-a")) {
            show_all = true;
        }
        if (strstr(flags, "-l")) {
            long_list = true;
        }
        // Handle special case where only '-' is given
        if (strcmp(flags, "-") == 0) {
            FILE* fd = fopen(hop_file2, "r");
            if (fd) {
                if (fgets(expanded_path, sizeof(expanded_path), fd)) {
                    expanded_path[strcspn(expanded_path, "\n")] = '\0'; // Remove newline
                } else {
                    snprintf(expanded_path, sizeof(expanded_path), ".");
                }
                fclose(fd);
            } else {
                perror("Error opening hop.txt");
                snprintf(expanded_path, sizeof(expanded_path), ".");
            }
        } else {
            expand_path(path, expanded_path);
        }
    } else {
        expand_path(path ? path : ".", expanded_path);
    }

    list_directory(expanded_path, show_all, long_list);
}

