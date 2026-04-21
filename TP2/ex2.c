#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(){
    pid_t pid; int* status = &status;


    if((pid = fork()) < 0) return -1;

    int valor;

    if(pid != 0){

        if(wait(NULL) < 0){
            return -1;
        }



    }

    printf("PID atual: %d\n" "PID Pai: %d\n", getpid(), getppid());

    return 0;






}