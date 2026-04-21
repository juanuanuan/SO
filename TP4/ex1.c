#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char **argv){
    int fd[2];
    pid_t pid;

    //f[0] -> read 
    //f[1] -> write

    if(pipe(fd) == -1){
        fprintf(stderr, "Pipe error: %s\n", strerror(errno));
        return 1;
    }

    pid = fork();

    if(pid < 0){
        fprintf(stderr, "Error with forking\n");
        return 1;
    } else if (pid > 0){
        close(fd[0]);
        int second;
        sleep(5);
        write(fd[1], &second, sizeof(int));
        fprintf(stdout, "Im the parent process, with id: %d, and im writing %d", getpid(), second);
        close(fd[1]);
        wait(NULL);
    } else {
        close(fd[1]);
        int first;
        read(fd[0], &first, sizeof(int));
        fprintf(stdout, "Im am the child process, with id: %d and im reading %d", getpid(), first);
        close(fd[0]);
    } 
    
    
    
    return 0;



}