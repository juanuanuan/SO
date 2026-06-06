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

