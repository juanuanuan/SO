#ifndef LIB_H
#define LIB_H


#include <stdbool.h>
#include <sys/types.h>
#include <time.h>

#define MAX_PARALLEL 2 // number of processes tha can run simultaniously
#define MAX_QUEUE 100
typedef struct {
    int notification_type; // 0 for new request, 1 for done task, 2 for wait, 3 to execute and for 4 - 5 default commands (-exit, -status)
    int running; // 0 to run, 1 for running
    bool canRun;
    int status; // process status
    pid_t pid; // Process Id
    char command[256]; // executable commands
    double start_time; // execution start time in seconds
    double end_time; // execution end time in seconds 
    int overall_fd; // generic file descriptor for hang purposes
} Programa;





typedef struct {
    Programa tasks[MAX_QUEUE];
    int size;
} Queue;

#endif


// Controller 

int enqueue(Queue *q, Programa new_task, int pipe_write_end);
int dequeue(Queue *queue, pid_t finished_pid, int pipe_write_end);
int process(int pipe_write_end, int pipe_read_end);
int manager(int pipe_read_end, Queue *queue, int pipe_write_end);

// Runner

void parser(char *command, char **argv, char **file_in, char **file_out, char **file_err);
void cutpipe(char *command, char **argv, int *argc);
void executer(char *command);
int dispatch(int argc, char **argv);
