#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#define SERVER_PORT "80"
#define BUFFER_SIZE 8192

// Function to strip HTML tags from the response
void stripHtmlTags(char *src, char *dest) {
    int in_tag = 0;  // Flag to indicate whether we are inside an HTML tag
    while (*src) {
        if (*src == '<') {
            in_tag = 1;  // Start of HTML tag
        } else if (*src == '>') {
            in_tag = 0;  // End of HTML tag
            src++;       // Move to the next character after '>'
            continue;    // Skip the '>'
        }

        // If not inside a tag, copy the character to the destination
        if (!in_tag && *src != '>') {
            *dest = *src;
            dest++;
        }
        src++;
    }
    *dest = '\0';  // Null-terminate the cleaned string
}

void fetchManPage(const char *command_name) {
    // Step 1: DNS resolution
    struct hostent *hst;
    if ((hst = gethostbyname("man.he.net")) == NULL) {
        perror("gethostbyname");
        exit(1);
    }

    // Step 2: Create a socket
    int socket_fd;
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    // Define server address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(80);
    server_addr.sin_addr = *((struct in_addr *)hst->h_addr);
    memset(&(server_addr.sin_zero), 0, 8);

    // Step 3: Connect to the server
    if (connect(socket_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) {
        perror("connect");
        exit(1);
    }

    // Step 4: Send HTTP GET request
    char request[BUFFER_SIZE];
    snprintf(request, sizeof(request), "GET /?topic=%s&section=all HTTP/1.1\r\nHost: man.he.net\r\n\r\n", command_name);
    if (send(socket_fd, request, strlen(request), 0) == -1) {
        perror("send");
        exit(1);
    }

    // Step 5: Receive and parse the response
    char response[BUFFER_SIZE];
    char cleaned_response[BUFFER_SIZE * 2];  // Buffer for stripped HTML content
    int bytes_received;

    while ((bytes_received = recv(socket_fd, response, sizeof(response) - 1, 0)) > 0) {
        response[bytes_received] = '\0';  // Null-terminate the response

        // Check if "No matches" is found in the response
        if (strstr(response, "No matches") != NULL) {
            printf("No matches found for '%s'. Printing full response (without HTML tags):\n", command_name);
            stripHtmlTags(response, cleaned_response);  // Strip HTML tags
            printf("%s\n", cleaned_response);  // Print the cleaned response
            break;  // Exit the loop early if no matches are found
        }

        // Check if the "NAME" section is found
        char *start = strstr(response, "NAME");
        if (start != NULL) {
            printf("Found 'NAME' section in the response. Printing cleaned response (without HTML tags):\n");
            stripHtmlTags(response, cleaned_response);  // Strip HTML tags
            printf("%s\n", cleaned_response);  // Print the cleaned response
            break;  // Exit the loop after printing the response
        }
    }

    if (bytes_received < 0) {
        perror("recv");
    }

    // Step 6: Close the socket
    close(socket_fd);
}

void Iman(char* command) {
    fetchManPage(command);
}

// int main(int argc, char *argv[]) {
//     if (argc != 2) {
//         fprintf(stderr, "Usage: %s <command>\n", argv[0]);
//         return 1;
//     }

//     Iman(argv[1]);
//     return 0;
// }
