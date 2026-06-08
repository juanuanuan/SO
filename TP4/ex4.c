#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>



int main(int argc, char **argv){ //imaginando que o processo filho envia ao processo pai uma string atraves de um pipe, e o processo pai quer encontrar uma palavra chave
    pid_t pid;
    int fd[2];

    if(pipe(fd) == -1){
        fprintf(stderr, "Piping error: %s\n", strerror(errno));
        return -1;
    }
    pid = fork();
    if(pid < 0){
        fprintf(stderr, "Forking error.\n");
        return -1;
    } else if (pid == 0){
        char *result = ("Im the child with id, and if my parent is reading this, he will return: SO.\n");
        close(fd[0]);
        write(fd[1], result, strlen(result) + 1);
        close(fd[1]);
    } else {
        close (fd[1]);
        char buffer[256];
        read(fd[0], buffer, sizeof(buffer)); // podiamos adicionar erros de read e write, como if (read...) == -1 {fprintf(stderr, "Erro  com leitura do ficheiro");}

        if(strstr(buffer, "SO") != NULL){
            fprintf(stdout, "Im the parent Precess with id %d, and im reading 'SO' in my child's message. \n", getpid());
        } else {
            fprintf(stdout, "Word not found, by parent Process.\n");
        }

        wait(NULL);
        close(fd[0]); // ta um bocado aldrabada esta função, mas acho que dá para perceber
    } return 0;
}