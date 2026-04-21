#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv){
    pid_t pid;
    

    if((pid = fork()) < 0) return -1;

    if(pid == 0){
        execlp("ls","ls", "-l", NULL);
    } 

    if(wait(NULL) == -1){
        return -1;
    }

    return 0;

}