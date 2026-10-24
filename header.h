#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <pwd.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <ctype.h>
#include <dirent.h>
#include <grp.h>
#include <time.h>
#include <fcntl.h>
#include<stdbool.h>
#include <errno.h>
#include <termios.h>
#include <limits.h>

#include <sys/time.h>
#include <signal.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>

#include"basicreq.h"
#include"input.h"
#include"hop.h"
#include"reveal.h"
#include"logs.h"
#include"execut.h"
#include"othercom.h"
#include"proclore.h"
#include"seek.h"
#include"myshrc.h"
#include"io.h"
#include"pipes.h"
#include"activities.h"
#include"ping.h"
#include"fgandbg.h"
#include"iMan.h"
#include"neonate.h"

#define MY_MAX_INPUT 4096

struct Backgroundprocess{
    int pid;
    char name[MY_MAX_INPUT];
    
};


extern struct Backgroundprocess bgp[100];

extern char* cmd_el;
extern int time_elapsed;
extern int log_flag;
extern int log2_flag;
extern int flag_pipe;
extern int redir;
extern int end_flag;
extern int fd_pid;
extern int z_flag;

#endif