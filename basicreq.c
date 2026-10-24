
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <pwd.h>
#include <sys/utsname.h>
#include "header.h"
int basic(){
    char cwd[MY_MAX_INPUT];
    struct passwd *pwd;
    struct utsname sysinfo;

    pwd=getpwuid(getuid());

    uname(&sysinfo);
    
    getcwd(cwd,MY_MAX_INPUT);
    if(time_elapsed>2){
        printf("<%s@%s:%s;%s:%d>",pwd->pw_name,sysinfo.nodename,cwd,cmd_el,time_elapsed);
        cmd_el=0;
        time_elapsed=0;

    }else{
        printf("<%s@%s:%s>",pwd->pw_name,sysinfo.nodename,cwd);

    }
    
    return 0;
}