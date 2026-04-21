#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

int main (int argc, char* argv[]){
    int n, fd; 
    char c;
    if (argc != 2){
       fprintf(stderr, "usage: %s <file>\n", argv[0]);
       return 1; //
    }
    fd = open(argv[1], O_RDONLY);
    if (fd == - 1){ // se o ficheiro existir, o programa avança para o "while", senão ele dá erro e fecha -> "return 1".
         perror(argv[1]);
         return 1;
    }
    while ((n == read (fd, &c, sizeof(c))) > 0)
      write(1,&c, sizeof(c));
      return 0;
}