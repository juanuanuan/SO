#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char **argv){
    pid_t pid; 
    int status;

    if(argc < 2){
        fprintf(stderr, "%s <command>\n", argv[0]);
        return 1;
    }

    if((pid = fork()) < 0) return -1;

    if(pid == 0){
         execl("/bin/sh", "sh", "-c", argv[1], NULL); // filho executa o bash
         fprintf(stderr, "execlp failed: %s\n", strerror(errno)); // em caso de falha, dá erro, com errno ("error number")
         exit (1); // saiu com código 1

    } else {
        wait(&status); // o processo pai espera pelo fim da execução do bash
        if(WIFEXITED(status)){
            fprintf(stdout, "Precess exited with code: %d\n", WEXITSTATUS(status));
        }
    } return 0;
}