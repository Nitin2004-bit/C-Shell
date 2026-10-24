#include"header.h"


#include <ctype.h>
#include <string.h>

char *trimwhitespace(char *str)
{
  char *end;

  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  // Write new null terminator character
  end[1] = '\0';

  return str;
}

// implement for multiple redirections as well 
// assumption: input file must be in the working directory else  invalid input file, for wc prints the input file as well
char* args[MY_MAX_INPUT];

void io_handler(char* command){
    
    int count=0;
    int original_stdin = dup(STDIN_FILENO);
    int original_stdout = dup(STDOUT_FILENO);
    // printf("command=%s\n",command);
    // printf("in\n");
    fflush(stdout);
    if(strstr(command,">")&&strstr(command,"<")){
        char* tok=strtok(command,"<");
        while(tok!=NULL){
            args[count]=tok;
            
            count++;
            tok=strtok(NULL," >\n");
        }
        int output_fd=-1;
        char* output_file=NULL;
        output_file=args[count-1];
        if(output_file){
            int flags = O_WRONLY | O_CREAT;
            output_fd=open(output_file,flags,0644);
            if(output_fd==-1){
                perror("opening output file");
                return;
            }
            dup2(output_fd,STDOUT_FILENO);
            close(output_fd);
        }
        char commd[MY_MAX_INPUT];
        snprintf(commd,MY_MAX_INPUT,"%s %s",args[0],args[1]);
        exect(commd);
        dup2(original_stdout,STDOUT_FILENO);
        close(original_stdout);

    }else if(strstr(command,">>")&&strstr(command,"<")){
        char* tok=strtok(command,"<");
        while(tok!=NULL){
            args[count]=tok;
            
            count++;
            tok=strtok(NULL," >\n");
        }
        int output_fd=-1;
        char* output_file=NULL;
        output_file=args[count-1];
        if(output_file){
            int flags = O_WRONLY | O_CREAT | O_APPEND;
            output_fd=open(output_file,flags,0644);
            if(output_fd==-1){
                perror("opening output file");
                return;
            }
            dup2(output_fd,STDOUT_FILENO);
            close(output_fd);
        }
        char commd[MY_MAX_INPUT];
        snprintf(commd,MY_MAX_INPUT,"%s %s",args[0],args[1]);
        exect(commd);
        dup2(original_stdout,STDOUT_FILENO);
        close(original_stdout);
    }else if(strstr(command,">>")){
        char* tok=strtok(command,">>");
        while(tok!=NULL){
            args[count]=tok;
            count++;
            // break;
            tok=strtok(NULL," >\n");

        }
        int output_fd=-1;
        char* output_file=NULL;
        output_file=args[count-1];
        if(output_file){
            int flags = O_WRONLY | O_CREAT | O_APPEND;
            output_fd=open(output_file,flags,0644);
            if(output_fd==-1){
                perror("opening output file");
                return;
            }
            dup2(output_fd,STDOUT_FILENO);
            close(output_fd);
        }
        exect(args[0]);
        dup2(original_stdout,STDOUT_FILENO);
        close(original_stdout);
    }else if(strstr(command,"<")){
        char* tok2=strtok(command,"<");
        while(tok2!=NULL){
            args[count]=tok2;
            count++;
            tok2=strtok(NULL,"\n");
        }
        int input_fd=-1;
        char* input_file=NULL;
        input_file=args[count-1];
        if(input_file){
            // printf("inputfile=%s\n",input_file);
            input_file=trimwhitespace(input_file);
            // printf("%s\n",input_file);
            int input_fd=open(input_file,O_RDONLY);
            if(input_fd==-1){
                perror("opening input file");
                return;
            }
            

            dup2(input_fd,STDIN_FILENO);
            close(input_fd);
            char buf[MY_MAX_INPUT];
            ssize_t bytes_read;

            // Read the file and print its contents
            // while ((bytes_read = read(STDIN_FILENO, buf, sizeof(buf) - 1)) > 0) {
            //     // Ensure buf is null-terminated
            //     buf[bytes_read] = '\0';
            //     printf("content=%s", buf);
            // }

        }
        
        
        char commd[MY_MAX_INPUT];
        // printf("%s %s\n",args[0],args[1]);
        snprintf(commd,MY_MAX_INPUT,"%s %s",args[0],args[1]);
        // exect(args[0]);
        args[0]=trimwhitespace(args[0]);
        // printf("%s1\n",args[0]);
        char *cmd_args[2] = {args[0], NULL};
        // execvp(cmd_args[0],cmd_args);
        exect(args[0]);
        dup2(original_stdin,STDIN_FILENO);
        close(original_stdin);
        // printf("2\n");
    }else if(strstr(command,">")){
        char* tok=strtok(command,">");
        while(tok!=NULL){
            args[count]=tok;
            count++;
            // break;
            tok=strtok(NULL," \n");

        }
        int output_fd=-1;
        char* output_file=NULL;
        output_file=args[count-1];
        if(output_file){
            int flags = O_WRONLY | O_CREAT;
            output_fd=open(output_file,flags,0644);
            if(output_fd==-1){
                perror("opening output file");
                return;
            }
            dup2(output_fd,STDOUT_FILENO);
            close(output_fd);
        }
        exect(args[0]);
        dup2(original_stdout,STDOUT_FILENO);
        close(original_stdout);

    }
}