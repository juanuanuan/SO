#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#define BUFFER_SIZE 100


int main (int argc, char* argv[]){
   int fd_origem = open("origem.txt", O_RDONLY);

   if(fd_origem == -1) return 1;

   ssize_t bytes_lidos, bytes_recebidos;

   char buffer[BUFFER_SIZE];

   int fd_destino = ("destino.txt", O_WRONLY | O_TRUNC | O_CREAT, 0600);

   if(fd_destino == -1) return 1;

   while ((bytes_lidos = read(fd_origem, buffer, BUFFER_SIZE)) > 0){
      if( write(fd_destino, buffer,bytes_lidos) == -1) return 1;
      write(fd_destino, buffer,bytes_lidos);
   }

   close (fd_destino);
   close (fd_origem);

   



}