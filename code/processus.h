#ifndef PROCESS_H
#define PROCESS_H

typedef struct {
    int pid;
    char name[64];
    double cpu;
    double mem;
} Process;

#endif
