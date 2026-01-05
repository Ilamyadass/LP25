#ifndef PROCESSUS_UI_H
#define PROCESSUS_UI_H

#include <stddef.h> // Pour size_t



// Type de connexion
typedef enum {
    CON_LOCAL,
    CON_SSH,
    CON_TELNET
} ConnectionType;

// --- STRUCTURES DE DONNEES ---

// Représente un processus unique 
typedef struct {
    int pid;
    char user[32];
    char cmd[256];
    float cpu;
    float mem;
    char time[32];
} ProcessInfo;

// Liste dynamique de processus
typedef struct {
    ProcessInfo *items; // Le tableau
    int count;          // Nombre actuel de processus
    int capacity;       // Taille maximale du tableau (malloc)
} ProcessList;

// Représente une machine (Locale ou Distante)
typedef struct {
    char name[64];     
    char hostname[64];  
    int port;
    char username[64];
    char password[64];
    ConnectionType type;
    int connected;      
    char error_msg[128];
} Host;

// --- PROTOTYPES DES FONCTIONS ---

// 1. Gestion de la mémoire (Dans process.c)
void init_process_list(ProcessList *list, int capacity);
void free_process_list(ProcessList *list);

// 2. Collecte des données (Dans process.c)
int process_collect_local(ProcessList *list);

// 3. Réseau (Dans network.c)
int network_collect_ssh(Host *host, ProcessList *list);
int network_collect_telnet(Host *host, ProcessList *list);
int network_pause_pid(Host *host, int pid);
int network_resume_pid(Host *host, int pid);

// 4. Interface Graphique (Dans processus_ui.c ou ui.c)
void ui_init();
void ui_close();
void ui_render(ProcessList *list, Host *host, int selected, int offset);
// Note : int au lieu de void pour gérer le retour d'erreur si besoin
int ui_handle_input(int key, ProcessList *list, int *selected, int *offset);
int ui_process_action(int key, ProcessInfo *p);

#endif

