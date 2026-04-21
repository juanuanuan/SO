#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(char* command, char **argv){
    pid_t pid; 
    int status;

    if((pid = fork()) < 0) return -1;

    if(pid == 0){

    }
}