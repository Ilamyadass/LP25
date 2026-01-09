#include "processus_ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int network_collect_ssh(Host *host, ProcessList *list) {
    char final_command[4096]; // Grand buffer pour être sûr
    
    // 1. La commande à exécuter SUR le serveur distant (ps...)
    // On demande le PID, User, Commande, CPU, Mem, Temps.
    char *remote_cmd = "ps -Ao pid,user,comm,pcpu,pmem,time --no-headers --sort=-pcpu | head -n 50";

    // 2. Construction de la commande SSH complète
    
    if (strlen(host->password) > 0) {
        // CAS AVEC MOT DE PASSE (utilise sshpass)
        snprintf(final_command, sizeof(final_command), 
            "sshpass -p '%s' ssh -p %d -o StrictHostKeyChecking=no -o ConnectTimeout=3 %s@%s \"%s\"", 
            host->password,
            host->port ? host->port : 22,
            host->username,
            host->hostname,
            remote_cmd
        );
    } else {
        // CAS SANS MOT DE PASSE (Clé SSH)(non testé)
        snprintf(final_command, sizeof(final_command), 
            "ssh -p %d -o StrictHostKeyChecking=no -o ConnectTimeout=3 %s@%s \"%s\"", 
            host->port ? host->port : 22,
            host->username,
            host->hostname,
            remote_cmd
        );
    }

    // 3. Exécution via popen (ouvre un tuyau de lecture)
    FILE *fp = popen(final_command, "r");
    
    if (fp == NULL) {
        snprintf(host->error_msg, sizeof(host->error_msg), "Echec ouverture pipe");
        return 0;
    }

    char line[512];
    int count = 0;
    
    // 4. Lecture du résultat ligne par ligne
    while (fgets(line, sizeof(line), fp) != NULL && count < list->capacity) {
        ProcessInfo *p = &list->items[count];
        
        // Nettoyage avant remplissage
        memset(p, 0, sizeof(ProcessInfo));

        // Analyse de la ligne 
        // ps renvoie :  PID USER COMM %CPU %MEM TIME
        int r = sscanf(line, "%d %31s %255s %f %f %31s", 
               &p->pid, p->user, p->cmd, &p->cpu, &p->mem, p->time);
        
        // Si on a bien lu les 6 colonnes, on valide
        if (r == 6) {
            count++;
        }
    }

    // 5. Fermeture et vérification
    int status = pclose(fp);
    
    // Si on n'a rien lu, c'est qu'il y a eu une erreur
    if (count == 0) {
        snprintf(host->error_msg, sizeof(host->error_msg), "Erreur ou vide (code %d)", status);
        return 0;
    }

    list->count = count;
    return 1; // Succès
}

/*---------------------------- FONCTIONS D'ACTIONS RESEAU------------------------------------- */


// 1. Fonction outil 
// Elle fabrique la commande SSH et l'envoie.
int network_exec_ssh(Host *host, char *remote_cmd) {
    char final_command[2048];
    
    // Construction de la commande SSH 
    if (strlen(host->password) > 0) {
        snprintf(final_command, sizeof(final_command), 
            "sshpass -p '%s' ssh -p %d -o StrictHostKeyChecking=no -o ConnectTimeout=3 %s@%s \"%s\"", 
            host->password, 
            host->port ? host->port : 22, 
            host->username, 
            host->hostname, 
            remote_cmd);
    } else {
        snprintf(final_command, sizeof(final_command), 
            "ssh -p %d -o StrictHostKeyChecking=no -o ConnectTimeout=3 %s@%s \"%s\"", 
            host->port ? host->port : 22, 
            host->username, 
            host->hostname, 
            remote_cmd);
    }

    // Exécution
    int ret = system(final_command);
    return (ret == 0); // system renvoie 0 si tout s'est bien passé
}

// 2. Vos fonctions d'actions (qui appellent la fonction outil)

int network_kill_pid(Host *host, int pid) {
    char cmd[64];
    snprintf(cmd, sizeof(cmd), "kill -9 %d", pid);
    return network_exec_ssh(host, cmd);
}

int network_pause_pid(Host *host, int pid) {
    char cmd[64];
    snprintf(cmd, sizeof(cmd), "kill -STOP %d", pid);
    return network_exec_ssh(host, cmd);
}

int network_stop_pid(Host *host, int pid) {
    char cmd[64];
    snprintf(cmd, sizeof(cmd), "kill -TERM %d", pid);
    return network_exec_ssh(host, cmd);
}

int network_resume_pid(Host *host, int pid) {
    char cmd[64];
    snprintf(cmd, sizeof(cmd), "kill -CONT %d", pid);
    return network_exec_ssh(host, cmd);
}

int network_kill_name(Host *host, char *name) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "pkill -9 %s", name);
    return network_exec_ssh(host, cmd);
}


