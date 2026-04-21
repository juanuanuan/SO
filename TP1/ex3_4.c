#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ex3_4.h"

#define FILENAME "lista_de_pessoas.txt"



int adicionaPessoa(char* nome, int* idade){
    int fd = open(FILENAME, O_WRONLY | O_TRUNC | O_CREAT, 0600 );
    if(fd == -1) return 1;

    struct Pessoa person;

    person.idade = idade;
    strcpy(person.nome, nome);

    ssize_t p_bytes = sizeof(person);

    ssize_t bytes_escritos = write(fd, &person, p_bytes);
    if(bytes_escritos < 0) return 1;

    printf("Pessoa adicionada. \n"
           "Nome: %s, \n"
           "Idade: %d. \n", person.nome, person.idade);

    close (fd);
    return 0;       
}

int removePessoa(char *nome, int* idade){
    int fd = open(FILENAME, O_WRONLY);
    if(fd == -1) return 1;

    int fd_temp = open("temp.txt", O_WRONLY | O_TRUNC | O_CREAT, 0600);
    if(fd == -1) {close(fd); return 1;}




    struct Pessoa person;

    person.idade = idade;
    strcpy(person.nome, nome);
    int pessoaEncontrada = 0;

    while(read(fd, &person, sizeof(struct Pessoa)) > 0){
        if(person.idade == idade && strcmp(person.nome,nome) == 0){
            pessoaEncontrada = 1;
            continue;
        }

        write(fd_temp, &person, sizeof(struct Pessoa));
    }

    close(fd);
    close(fd_temp);
    return 0;

    
}

int listaPessoas(int N){
    int fd = open(FILENAME, O_WRONLY | O_TRUNC | O_CREAT, 0600);
    if(fd == 1) return 1;

    struct Pessoa person;
    for(int i = 0; i < N; i ++){
        ssize_t bytes_lidos = read(fd, &person, sizeof(person));
        if(bytes_lidos < 0) return -1;

        printf("Pessoa %d: \n", i+1);
        printf("NNome da pessoa %s:\n", person.nome);
        printf("Idade da Pessoa %d: %d.\n", i+1, person.idade);
    }

    close (fd);
    return 0;


}

int atualizaIdade(char* nome, int* idade){
    int fd = open(FILENAME, O_WRONLY | O_TRUNC | O_CREAT, 0600);
    if(fd == -1) return 1;

    struct Pessoa person;
    person.idade = idade;
    strcpy(person.nome, nome);
    int encontrou = 0;

    ssize_t bytes_lidos;

    while(bytes_lidos = read(fd, &person, sizeof(struct Pessoa))){
        if(bytes_lidos < 0 || bytes_lidos != sizeof(struct Pessoa)){ close(fd); return -1;}
        if(strcmp(person.nome,nome) == 0){
            person.idade = idade;
            if(lseek(fd,-sizeof(struct Pessoa), SEEK_CUR) == -1){
                close(fd);
                return -1;
            }

            if(write(fd, &person, sizeof(struct Pessoa)) != sizeof (struct Pessoa)){
                close (fd);
                return -1;
            }

            encontrou = 1;
            break;


        
        }

        close(fd);
        return encontrou;

    }
}



int posicao(int posicao, int* idade){
    int fd = open(FILENAME, O_WRONLY, 0600);
    if (fd == -1) return 1;

    struct Pessoa person;
    int encontrou = 0;
    ssize_t bytes_lidos;
    int i = 1;


    while(bytes_lidos = read(fd,&person,sizeof(Pessoa)) > 0){
        if(bytes_lidos != sizeof(Pessoa)) { close(fd); return -1;}
        if(i == posicao){
            person.idade = idade;

            if(lseek(fd,sizeof(Pessoa), SEEK_CUR == -1)){
                close (fd);
                return -1;
            }

            if(write(fd,&person,sizeof(Pessoa)) == -1){
                close(fd);
                return -1;
            }

            encontrou = 1;
            break;
        } i ++; 

    } close(fd);
    return encontrou;

}

