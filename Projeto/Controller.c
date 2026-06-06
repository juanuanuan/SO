#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <sys/stat.h>
#include "lib.h"

int activeP = 0;



int enqueue(Queue *q, Programa new_task, int pipe_write_end){

    if(q -> size >= MAX_QUEUE){
       new_task.notification_type = -1;
       write(pipe_write_end, &new_task, sizeof(Programa));
       return -1;
    } 
        q -> tasks[q->size] = new_task;
        q -> size ++;
                                 // enqueueAux(&q, new_t) escalonamento 
        if (activeP < MAX_PARALLEL){  // busy < maxParallel -> avisar process que pode executar task (assinal task como a correr etc...) (incrementar busy)
            q -> tasks[q -> size -1].running = 1;
            q -> tasks[q -> size -1].notification_type = 3;
            activeP ++; 
            write(pipe_write_end, &q -> tasks[q -> size - 1], sizeof(Programa));

        } else {
            q -> tasks[q -> size - 1].running = 0;
            q -> tasks[q -> size - 1].notification_type = 2;
            write(pipe_write_end, &q -> tasks[q -> size - 1], sizeof(Programa));
        }
    return 0;
}

int dequeue(Queue *queue, pid_t finished_pid, int pipe_write_end){
    int found_index = -1;
    for(int i = 0; i < queue -> size; i ++){
        if(queue -> tasks[i].running == 1 && queue -> tasks[i].pid == finished_pid){
        found_index = i;
        break;
        }
    }


    if(found_index ==  -1){
        return -1;
    }

    for(int i = found_index; i < queue -> size - 1; i ++){
        queue -> tasks[i] = queue -> tasks[i + 1];

    } 
    activeP --;
    queue -> size --;
    int pendent = 0;
    for(int i = 0; i < queue -> size; i++ ){
        if(queue -> tasks[i].running == 0){
            queue -> tasks[i].running = 1;
            queue -> tasks[i].notification_type = 3;
            activeP ++;
            write(pipe_write_end, &queue -> tasks[i], sizeof(Programa));
            pendent = 1;
            break;
        }
    }

    if(pendent == 0){
        Programa dummySet;

        memset(&dummySet, 0, sizeof(Programa));
        dummySet.pid = -1;
        write(pipe_write_end, &dummySet, sizeof(Programa));
    }

    return 0;
}


 int process(int pipe_write_end, int pipe_read_end){
    int fd = open("temp/Communicator", O_RDONLY);
    if(fd == -1){
        fprintf(stderr, "Error opening the main FIFO - Communicator: %s", strerror(errno));
        return -1;
    }

    Programa toSend_command;
    memset(&toSend_command, 0, sizeof(Programa));
    if(read(fd, &toSend_command, sizeof(Programa)) > 0){
        if(toSend_command.notification_type == 4){
        fprintf(stdout, "Server is shutting down.\n");
        fflush(stdout);
        close(fd);
        unlink("temp/Communicator");
        _exit(0);
        
    }


    write(pipe_write_end, &toSend_command, sizeof(Programa));
    }


    Programa return_command;
    memset(&return_command, 0, sizeof(Programa));
    return_command.canRun = true;

    int read_bytes = read(pipe_read_end, &return_command, sizeof(Programa));
    if(read_bytes <= 0){
        fprintf(stderr, "Error reading upcoming task: %s", strerror(errno));
        return -1;
    }

    if(toSend_command.notification_type == 5){
        char client_fifo[256];
        snprintf(client_fifo, sizeof(client_fifo), "temp/comm_%d", toSend_command.pid);

        int client_fd = open(client_fifo, O_WRONLY);
        if(client_fd != -1){
            write(client_fd, &return_command, sizeof(Programa));
            while(return_command.notification_type != 6){
                read(pipe_read_end, &return_command, sizeof(Programa));
                write(client_fd, &return_command, sizeof(Programa));
            }
            close(client_fd);
        } else{
            perror("Error opening client FIFO");
        }
    }

    

    else if(return_command.pid != -1){


    char client_fifo[256];
    snprintf(client_fifo, sizeof(client_fifo), "temp/comm_%d", return_command.pid);

    int fd2 = open(client_fifo, O_RDWR);
    if(fd2 != -1){
        write(fd2, &return_command, sizeof(Programa));
        close(fd2);
    } else{
        fprintf(stderr, "\n Fatal Error on: %s. Reason of Error: %s\n", client_fifo, strerror(errno));
        }
    } else {
        printf("End task confirmation.\n");
        fflush(stdout);
    }

    

    return 0;

 }

// FIXME: ter atenção ao manager, melhorar lógica 

 int manager(int pipe_read_end, Queue *queue, int pipe_write_end){
    Programa tasks;
    memset(&tasks, 0, sizeof(Programa));

    int read_bytes = read(pipe_read_end, &tasks, sizeof(Programa));
    if(read_bytes == 0){
        fprintf(stderr, "EOF (end of file).\n");
        exit(1);
    }
    else if(read_bytes < 0){
        fprintf(stderr, "[Process - Manager] Reading error: %s\n", strerror(errno));
        exit(0);
    }
    if(tasks.notification_type == 0){
            enqueue(queue, tasks, pipe_write_end);
    } else if(tasks.notification_type == 1){
            dequeue(queue, tasks.pid, pipe_write_end);
    }

    if(tasks.notification_type == 5){
        Programa status_info;
        for(int i = 0; i < queue -> size; i ++){
                memset(&status_info, 0, sizeof(Programa));
                status_info.notification_type = queue->tasks[i].notification_type;
                status_info.pid = queue->tasks[i].pid;
                strcpy(status_info.command, queue->tasks[i].command);

                write(pipe_write_end, &status_info, sizeof(Programa));
        }

        memset(&status_info, 0, sizeof(Programa));
        status_info.notification_type = 6;
        write(pipe_write_end, &status_info, sizeof(Programa));
    }
 return 0;
 }



int main(){
    int pipe1[2];
    int pipe2[2];
    Queue *queue = malloc(sizeof(Programa));
    if(queue == NULL) return -1;
    queue -> size = 0;

    mkdir("temp", 0700);
    if(mkfifo("temp/Communicator", 0666) == -1){
        if(errno == EEXIST){
            fprintf(stdout, "Communicator is open.\n");
        } else {
            fprintf(stderr, "Error creating the main FIFO - Communicator: %s", strerror(errno));
            return -1;
        }
    }



    if(pipe(pipe1) == -1){
        fprintf(stderr, "Pipe1 Error: %s", strerror(errno));
        return -1;
    }

    if(pipe(pipe2) == -1){
        fprintf(stderr, "Pipe2 Error: %s", strerror(errno));
        return -1;
    }

    printf("Pipe1: Read=%d, Write=%d | Pipe2: Read=%d, Write=%d\n", pipe1[0], pipe1[1], pipe2[0], pipe2[1]);
    fflush(stdout);


    pid_t pid1 = fork();

    if(pid1 < 0){
        fprintf(stderr, "Forking Error: %s", strerror(errno));
        return -1;
    } else if (pid1 == 0){
        close(pipe1[0]); close(pipe2[1]); //writter

        int fd_comm = open("temp/Communicator", O_RDWR);
        if(fd_comm == -1){
            fprintf(stderr, "Error opening the main FIFO: %s", strerror(errno));
            exit(1);
        }
        while(1){
            process(pipe1[1], pipe2[0]);
        }
    } 


    pid_t pid2 = fork();
    if(pid2 < 0){
        fprintf(stderr, "Forking Error: %s", strerror(errno));
        return -1;
    } else if(pid2 == 0){
        close(pipe1[1]); close(pipe2[0]);
        while(1){
            manager(pipe1[0], queue, pipe2[1]);
        }
    }

    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);

    wait(NULL);
    wait(NULL);

    return 0;
    


  
}

