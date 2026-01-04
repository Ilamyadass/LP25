#ifndef PROCESS_H
#define PROCESS_H

#include "processus_ui.h" // Nécessaire pour ProcessList et ProcessInfo

/**
 * Initialise une liste de processus (Allocation mémoire).
 * @param list : Pointeur vers la structure ProcessList.
 * @param capacity : Nombre maximum de processus à stocker (ex: 1024).
 */
void init_process_list(ProcessList *list, int capacity);

/**
 * Libère la mémoire allouée pour la liste.
 * @param list : Pointeur vers la structure ProcessList.
 */
void free_process_list(ProcessList *list);

/**
 * Scanne le répertoire /proc pour récupérer les processus en cours
 * sur la machine LOCALE.
 * @param list : La liste à remplir.
 * @return 1 si succès, 0 si erreur critique.
 */
int process_collect_local(ProcessList *list);

#endif