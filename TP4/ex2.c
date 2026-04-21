#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

int main (int argc, char **argv){
    int fd[2];
    pid_t pid;

    if(pipe(fd) == -1){
        fprintf(stderr, "Piping error: %s", strerror(errno));
        return -1;
    }

    pid = fork();

    if(pid < 0){
        fprintf(stderr, "Forking error.\n");
        return -1;
    } else if(pid == 0){
        int first;
        close(fd[0]);
        write(fd[1], &first, sizeof(int));
        fprintf(stdout, "Im the child process with id %d, and im sending to my parent %d", getpid(), first);
        close(fd[1]);
    } else {
        int second;
        close(fd[1]);
        sleep(5);
        read(fd[0], &second, sizeof(int));
        fprintf(stdout, "Im the child parent with id %d, and im recieving to my child %d", getpid(), second );
        wait(NULL);
        close(fd[0]);
    }

    return 0;
}
