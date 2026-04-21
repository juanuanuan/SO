#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **argv){
    execlp("ls", "ls", "-ls", NULL); 
    printf("O programa nunca chega aqui :)\n");
    return 0;
}