#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include "lib.h"
#include <wordexp.h>


void parser(char *command, char **argv, char **file_in, char **file_out, char **file_err){ //crate a fun called parser, to devide the string of commands in smaller peaces for, mainly, pipeline execution 
    char *token = strtok(command, " "); //tokenizing the string of commands, to be able to execute properly
    int num_tokens = 0;

    *file_in = NULL; //files for std in, out and error
    *file_out = NULL;
    *file_err = NULL;

    while(token != NULL){ // token check
        if(strcmp(token, "<") == 0){
            token = strtok(NULL, " ");
            *file_in = strdup(token);
        } else if(strcmp(token, ">") == 0){
            token = strtok(NULL, " ");
            if(token!= NULL) *file_out = strdup(token);
        } else if(strcmp(token, "2>") == 0){
            token = strtok(NULL, " ");
            *file_err = strdup(token);
        } else {
            argv[num_tokens] = strdup(token);
            num_tokens ++;
        }

        token = strtok(NULL, " ");

    }

    argv[num_tokens] = NULL; // ensure that the last command execvp reads is NULL

}

void cutpipe(char *command, char **argv, int *argc){ // create a fun tha divides a string of commands only when a anonym pipeline appears: "|"
    int i = 0;
    char *token = strtok(command, "|"); 

    while(token != NULL){
        argv[i] = token;
        i ++;  
        token = strtok(NULL, "|");
    }

    argv[i] = NULL;
    *argc = i;
}

void executer(char *command){ // the fun .executer() is only for comand execution, as the system supports pipeline execution, a function is made to handle all command varieties from a simple "ls" to a more complex one
    char *commands[20]; //list of commands
    int nCommands = 0; // number of commands 
    int in_fd = 0; // input file discriptor is opened. allways. 
    int fd[2]; //pipeline for execution 
    pid_t pid;
    int status; //process status

    cutpipe(command, commands, &nCommands); //cutpipe is called first to check if there is a pipeline in the commmand, if there is, it devides it 

    for(int i = 0; i < nCommands; i ++){ // with the command devided in possibly, "|", it beggins to check the string for portions of executable commands.
        if(i < nCommands - 1){
            pipe(fd); // if its not the last command, there can still be a pipeline command, or not. 
        }
        pid = fork();

        if(pid < 0){
            perror("Forking Error");
        } else if (pid == 0){
            if(in_fd != 0){
                dup2(in_fd, 0); 
                close(in_fd);
            }

            if(i < nCommands - 1){
                dup2(fd[1], 1);
                close(fd[0]);
                close(fd[1]);
            }

            char *args[64]; //list of arguments
            char *file_in, *file_out, *file_err;

            parser(commands[i], args, &file_in, &file_out, &file_err); //passing all to the parser to search for other redirections of stdouts

            if(file_in != NULL){
                int fd_in = open(file_in, O_RDONLY);
                dup2(fd_in, 0);
                close(fd_in);
            }

            if(file_out != NULL){
                int fd_out = open(file_out, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                dup2(fd_out, 1);
                close(fd_out);
            }

            if(file_err != NULL){
                int fd_err = open(file_err, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                dup2(fd_err, 2);
                close(fd_err);
            }

            if(args[0] == NULL){
                exit(0);
            }

            execvp(args[0], args);
            perror("Comand not found");
        } else {
            if(in_fd != 0){
                close(in_fd);
            }

            if(i < nCommands - 1){
                close(fd[1]);
            }

            in_fd = fd[0];

        }
    }

    for(int i = 0; i < nCommands; i ++){
        wait(&status);
    }

}

int dispatch(int argc, char **argv){ //creates a temp fifo for every upcoming client, writes the request to the .process() and recieves the aprprovall(or not) from him
    char fifo[100];
    int pid = getpid();

    sprintf(fifo, "temp/comm_%d", pid);

    if(mkfifo(fifo,0600) == -1){
        
        if(errno == EEXIST){
        perror("File already exists");
     } else {
        perror("Error creating the temp FIFO");
     }
    }


    Programa request;
    request.pid = pid;
    request.notification_type = 0;
    request.status = 0;
    snprintf(request.command,sizeof(request.command), "%s", argv[1]); //storing information on the struct caller "request" on a buffer


    bool bounced = true; // condition that makes the server allways online.

    while(bounced == true){

    int server_fd = open("temp/Communicator", O_WRONLY);

    if(server_fd == -1){
        perror("Could not open the Communicator.");
        unlink(fifo);
        return -1;
    }


    if(write(server_fd, &request, sizeof(Programa)) == -1){
        perror("Error sending to the server");
        close(server_fd);
        unlink(fifo);
        return -1;
    }
     
    fprintf(stdout, "[%d] Command has been sent: %s\n",request.pid, request.command);
    
    close(server_fd);


    Programa response;

    int client_fd = open(fifo, O_RDONLY);
    read(client_fd, &response, sizeof(Programa));
    if(client_fd == -1){
        fprintf(stderr, "Could not open temp FIFO: %s", strerror(errno));
        unlink(fifo);
        return -1;
    }

    close(client_fd);

    if(response.notification_type == 2){
        fprintf(stdout, "Queue is full. Wait.\n");
        fflush(stdout);

        client_fd = open(fifo, O_RDONLY);
        read(client_fd, &response, sizeof(Programa));
        close(client_fd);
    }

    if(response.notification_type == 3){
        bounced = false;

    } 

    if(response.notification_type == 5){
        char comando[256];

    }

    }

    unlink(fifo);
    return 0;
}

int main(int argc, char **argv){
    struct timeval start, end;
    pid_t pid = getpid();


    if(argc < 2){
        fprintf(stderr, "Usage: %s <commands> missing args\n", argv[0]);
        return -1;
    }



    if(strcmp(argv[1],"-exit") == 0){
        Programa sender1;
        memset(&sender1, 0, sizeof(Programa));
        sender1.notification_type = 4;
        sender1.pid = getpid();

        int server_fd = open("temp/Communicator", O_WRONLY);
        write(server_fd, &sender1, sizeof(Programa));
        close(server_fd);

    }

    if(strcmp(argv[1], "-status") == 0){
        Programa sender2;
        memset(&sender2, 0, sizeof(Programa));
        sender2.notification_type = 5;

        char client_fifo[100];
        sprintf(client_fifo, "temp/comm_%d", sender2.pid);
        if(mkfifo(client_fifo, 0666) == -1){
            perror("Error creating temp FIFO");
            return -1;
        }

        int server_fd = open("temp/Communicator", O_WRONLY);
        write(server_fd, &sender2, sizeof(Programa));
        close(server_fd);

        int client_fd = open(client_fifo, O_RDONLY);
        if(client_fd == -1){
            perror("Error opening the FD");
            unlink(client_fifo);
            return -1;
        }

        Programa response;

        fprintf(stdout, "Server State\n");

        while(read(client_fd, &response, sizeof(Programa)) > 0){
            if(response.notification_type == 6){
                break;
            }

            if(response.notification_type == 2){
                fprintf(stdout, "On waiting line: %d | Command: %s\n", response.pid, response.command);
            } else if(response.notification_type == 3){
                fprintf(stdout, "With permission of execution: %d | Command: %s\n", response.pid, response.command);
            }
        }

        close(client_fd);
        unlink(client_fifo);

        return 0;

    }


    if(dispatch(argc, argv) == -1){
        perror("Fatal Error");
        return -1;
    }

    gettimeofday(&start, NULL);

    pid = fork();
    Programa done_task;
    int i;

    int status; 

    char actual_command[256];
    actual_command[0] = '\0';

    for(int i = 1; i < argc; i ++){
        strncat(actual_command, argv[i], sizeof(actual_command) - strlen(actual_command) - 1);
        if(i < argc - 1){
            strncat(actual_command, " ", sizeof(actual_command) - strlen(actual_command) - 1);
        }
    }

    

    if(pid < 0){
        perror("Forking Error");
        return -1;
    } else if(pid == 0){
        executer(actual_command);
        exit(1);
    } else {

    waitpid(pid, &status, 0);
    gettimeofday(&end, NULL);
    done_task.running = 0;
    done_task.pid = getpid();
    done_task.notification_type = 1;
    done_task.status = WEXITSTATUS(status);
    snprintf(done_task.command, sizeof(done_task.command), "%s", actual_command);

    done_task.end_time = (end.tv_sec - start.tv_sec) * 1000 + (end.tv_usec - start.tv_usec) / 1000;

    int server_fd = open("temp/Communicator", O_WRONLY); 
    if(server_fd == -1){
        perror("Error opening Communictaor to notify Server");
        return -1;
    }
    
    write(server_fd, &done_task, sizeof(Programa));
    close(server_fd);
    fprintf(stdout, "Run time of: %2.f milliseconds.\n", done_task.end_time);
    fflush(stdout); // insure that the message is printed with full guarantee.
    }
    return 0;


}

