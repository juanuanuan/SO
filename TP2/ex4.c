#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int main(){
    pid_t pid; int status;

    for(int i = 0; i <= 10; i ++){
        if((pid = fork()) < 0) _exit(EXIT_FAILURE);
        pid = fork();
        if(pid == 0) {
            exit(i);
        } else{ 
        pid_t codigo_filho = wait(&status);
        if(codigo_filho < 0) _exit(EXIT_FAILURE);

        if(WIFEXITED(status)){
            int e_status = WEXITSTATUS(status);
            printf("O Processo filho %d terminou com código %d.\n", codigo_filho, e_status);
        } else {
            printf("O Processo filho %d não terminou.\n", codigo_filho);
        }
     }
    } return 0;
}