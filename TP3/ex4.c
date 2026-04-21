#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>


int main (int argc, char **argv){ //fiz tudo na main para poupar trabalho, nem sei se é assim que se faz 
    pid_t pid; //inicializamos o process id e o status.
    int status = 0;

    if(argc < 2) {
        fprintf(stderr, "insufficient args %s\n", argv[0]);
    } // se os argumentos do bash forem insuficientes, o programa falha

    int n = argc - 1; // n é igual ao numero de argumentos
    int *counter = calloc(n,sizeof(int)); //counter é dado por um calloc (semelhante ao malloc, mas opera com mais bytes). 

    while (WIFEXITED(status) && WEXITSTATUS(status) != 0){ // enquanto o código de saida for diferente de zero, o programa avança pra uma re-execução. o processo só termina com código de saída igual a zero!
         for(int i = 0; i < n; i ++){ //precorremos o número de comandos (precessos a executar neste caso)

            pid = fork(); // vamos dar fork para inicilizar um processo filho, capaz de re-executar os processos.

            if(pid < 0){ // se o process id for menor que zero, isso significa nao existe processo filho.
                fprintf(stderr, "Fork has failed.\n", argv[0], NULL); //fprintf com standard error.
                return 1;
            } else if(pid == 0){ // aqui sabemos que estamos no processo filho
                execl(argv[i + 1], argv[i + 1], NULL); // o processo filho executa os comandos um a um.
                fprintf(stderr, "execl failed %s\n", strerror(errno)); // o programa só chega a este ponto se o exec falhar.
                exit(1); 
            }

            else { // se for o processo pai, vai esperar que o filho acabe de processar os comandos, pois se o porcesso pai não esperar, morre e dá crash ao programa.
                wait(&status);
                counter[i] ++; //esperando, e o processo filho executando com sucesso o comando, o contador incrementa para o número de comandos executados
                } 
        

            }
         }

        for(int i = 0; i < n; i++){ // para os comandos executados com sucesso, damos fprintf comn standard output.
            fprintf(stdout, "Precess %s executed %d times", argv[i + 1], counter[i]);
        }

        return 0;
    } 



