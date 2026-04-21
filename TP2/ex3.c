#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(){
    pid_t pid; int status;

    int i = 0;

    if((pid = fork()) < 0) _exit(EXIT_FAILURE);

   for(i = 0; i <= 10; i ++){

    if(pid == 0){
        pid = fork(); // como é sequencialmente, quando um processo faz fork, vai logo embora com o respetivo código.
        exit(i);
 
    } else {
        pid_t filho_pid = wait(&status);
        if(filho_pid == -1) _exit(EXIT_FAILURE);

        if(WIFEXITED(status)){
            int e_status = WEXITSTATUS(status);
            printf("O Processo filho %d terminou com código %d.\n", filho_pid, e_status);
        } else {
            printf("O processo filho %d não terminou corretamente.\n", filho_pid);
        }
    }
   } printf("Análise terminada.\n");
   return 0;
}