#ifndef PROCESS_H
#define PROCESS_H

#include "processus_ui.h" // Nécessaire pour ProcessList et ProcessInfo


void init_process_list(ProcessList *list, int capacity);

void free_process_list(ProcessList *list);

int process_collect_local(ProcessList *list);

#endif
