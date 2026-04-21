#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

typedef struct Pessoa{
    char nome[100];
    int idade;

} Pessoa;

int adicionaPessoa(char* nome, int* idade);
int removePessoa(char*nome, int* idade);
int listaPessoas(int N);
int atualizaIdade (char* nome, int* idade);
int posicao(int posicao, int* idade);

