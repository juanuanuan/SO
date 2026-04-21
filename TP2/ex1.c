#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main (){
    pid_t pid; int status;
    pid_t ppid;
   
    pid = fork();

    if(pid < 0) {
        return -1;

    } else if(pid == 0){
        pid = getpid();
        exit(10);
    } else {ppid = getppid(); exit (9);
    
    wait(&status);
    if(WIFEXITED(status)){
        printf("Status do processo pai: %d\n", WEXITSTATUS(status));
    }
    
    }

    printf("Process IDentifier do pai: %d \n", ppid);
    printf("Process IDentifier do filho: %d \n", pid); 



    return 0;
}


