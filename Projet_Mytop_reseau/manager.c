#include "processus_ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/stat.h>
#include <curses.h> // Pour beep()

#define MAX_HOSTS 10

Host hosts[MAX_HOSTS];
int host_count = 0;

// Ajoute une machine à la liste
void add_host(const char *name, const char *hostname, int port, const char *user, const char *pass, ConnectionType type) {
    if (host_count >= MAX_HOSTS) return;
    Host *h = &hosts[host_count++];
    strncpy(h->name, name, 63);
    strncpy(h->hostname, hostname, 63);
    h->port = port;
    strncpy(h->username, user, 63);
    strncpy(h->password, pass, 63);
    h->type = type;
    h->connected = 0;
    strcpy(h->error_msg, "En attente...");
}

// Vérifie la sécurité du fichier config (doit être 600)
int check_config_perms(const char *path) {
    struct stat st;
    if (stat(path, &st) != 0) return 0;
    if ((st.st_mode & 0077) != 0) {
        fprintf(stderr, "\n[SECURITE] Le fichier %s doit avoir les droits 600 (chmod 600 %s)\n", path, path);
        sleep(2);
        return 0;
    }
    return 1;
}

// Charge la configuration depuis un fichier
void load_config(const char *path) {
    if (!check_config_perms(path)) {
        printf("Fichier config ignoré.\n");
        return;
    }
    
    FILE *f = fopen(path, "r");
    if (!f) return;

    char line[256];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char *name = strtok(line, ":");
        char *ip = strtok(NULL, ":");
        char *port_str = strtok(NULL, ":");
        char *user = strtok(NULL, ":");
        char *pass = strtok(NULL, ":");
        char *type_str = strtok(NULL, ":");

        if (name && ip && port_str && user && type_str) {
            ConnectionType t = (strcmp(type_str, "ssh") == 0) ? CON_SSH : CON_TELNET;
            add_host(name, ip, atoi(port_str), user, pass ? pass : "", t);
        }
    }
    fclose(f);
}

// Fonction d'aide
void print_usage(char *prog_name) {
    printf("\n=== AIDE MYTOP ===\n");
    printf("Ce programme permet de surveiller les processus locaux et distants.\n\n");
    
    printf("VOICI LES 3 UTILISATIONS POSSIBLES :\n");
    
    printf("\n  1. LE CLASSIQUE (Local uniquement)\n");
    printf("     Lancez simplement le programme pour voir votre machine.\n");
    printf("     %s\n", prog_name);

    printf("\n  2. LE FICHIER DE CONFIG (Recommandé)\n");
    printf("     Charge une liste de serveurs depuis un fichier sécurisé.\n");
    printf("     (Ajoutez -a pour voir aussi votre machine locale)\n");
    printf("     %s -c .config_serveurs -a\n", prog_name);

    printf("\n  3. LA CONNEXION DIRECTE (Manuel)\n");
    printf("     Pour tester une connexion sans créer de fichier.\n");
    printf("     %s -s X.X.X.X -l user -p password\n", prog_name);

    printf("\nOPTIONS DISPONIBLES :\n");
    printf("  -h, --help       Affiche ce message d'aide\n");
    printf("  -c <fichier>     Chemin du fichier de configuration\n");
    printf("  -s <ip>          Adresse IP du serveur distant\n");
    printf("  -l <user>        Nom d'utilisateur SSH\n");
    printf("  -p <pass>        Mot de passe SSH\n");
    printf("  -a, --all        Ajoute 'Cette Machine' à la liste des serveurs\n");
    printf("\n");
}

int main(int argc, char **argv) {
    int opt;
    char *config_path = NULL;
    char *remote_server = NULL;
    char *login_str = NULL;
    char *username = NULL;
    char *password = "";
    int mode_all = 0;

    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"remote-config", required_argument, 0, 'c'},
        {"remote-server", required_argument, 0, 's'},
        {"login", required_argument, 0, 'l'},
        {"password", required_argument, 0, 'p'},
        {"all", no_argument, 0, 'a'},
        {0, 0, 0, 0}
    };

    // 1. Analyse des arguments
    while ((opt = getopt_long(argc, argv, "hc:s:l:p:a", long_options, NULL)) != -1) {
        switch (opt) {
            case 'h': print_usage(argv[0]); // <--- C'est ici qu'on appelle la belle aide !
                exit(0);
            case 'c': config_path = optarg; break;
            case 's': remote_server = optarg; break;
            case 'l': login_str = optarg; break;
            case 'p': password = optarg; break;
            case 'a': mode_all = 1; break;
        }
    }

    // 2. Chargement Config Fichier
    if (config_path) load_config(config_path);

    // 3. Ajout Manuel (Ligne de commande)
    if (remote_server) {
        if (!username && login_str) username = login_str;
        if (!username) username = getenv("USER");
        add_host("Serveur Manuel", remote_server, 22, username, password, CON_SSH);
    }
    
    // 4. Ajout Local (si demandé ou si liste vide)
    if (host_count == 0 || mode_all) {
        add_host("Cette Machine", "127.0.0.1", 0, "local", "", CON_LOCAL);
    }

    // --- INITIALISATION UI ---
    ui_init();
    
    ProcessList list;
    init_process_list(&list, 2048); // Capacité de 2048 processus max

    int current_host_idx = 0;
    int selected = 0;
    int offset = 0;
    int counter = 0;
    int ch;

    // --- BOUCLE PRINCIPALE ---
    while (1) {
        Host *h = &hosts[current_host_idx];

        // Mise à jour des données (toutes les 10 boucles soit ~1sec)
        if (counter % 10 == 0) {
            int ok = 0;
            if (h->type == CON_LOCAL) {
                ok = process_collect_local(&list);
            } else if (h->type == CON_SSH) {
                ok = network_collect_ssh(h, &list);
            }
            h->connected = ok;
        }

        // Affichage
        ui_render(&list, h, selected, offset);

        // Gestion Clavier (timeout 100ms)
        timeout(100); 
        ch = getch();

        if (ch == 'q') break;

        // F2 / F3 : Changer de machine
        if (ch == KEY_F(2)) { 
            current_host_idx--; 
            if (current_host_idx < 0) current_host_idx = host_count - 1;
            selected = 0; offset = 0; // Reset sélection
            // On force un rafraichissement immédiat des données
            counter = -1; 
        }
        if (ch == KEY_F(3)) { 
            current_host_idx++; 
            if (current_host_idx >= host_count) current_host_idx = 0;
            selected = 0; offset = 0;
            counter = -1;
        }

        // ACTIONS SUR PROCESSUS (Kill, Pause...)
        // Sécurité : Uniquement si la connexion est LOCALE
        if (list.count > 0 && selected < list.count) {
            ProcessInfo *p = &list.items[selected];
            int ok = 0;
        
            switch (ch) {
        
                case KEY_F(5): // PAUSE
                    if (h->type == CON_LOCAL) {
                        ok = ui_process_action(ch, p);
                    } 
                    else if (h->type == CON_SSH) {
                        ok = network_pause_pid(h, p->pid);
                    }
                    if (!ok) beep();
                    break;
        
                case KEY_F(6): // STOP (SIGTERM)
                    if (h->type == CON_LOCAL) {
                        ok = ui_process_action(ch, p);
                    } 
                    else if (h->type == CON_SSH) {
                        ok = network_kill_pid(h, p->pid); // SIGTERM ou SIGKILL selon ton choix
                    }
                    if (!ok) beep();
                    break;
        
                case KEY_F(7): // KILL (SIGKILL)
                    if (h->type == CON_LOCAL) {
                        ok = ui_process_action(ch, p);
                    } 
                    else if (h->type == CON_SSH) {
                        ok = network_kill_pid(h, p->pid);
                    }
                    if (!ok) beep();
                    break;
        
                case KEY_F(8): // RESUME
                    if (h->type == CON_LOCAL) {
                        ok = ui_process_action(ch, p);
                    } 
                    else if (h->type == CON_SSH) {
                        ok = network_resume_pid(h, p->pid);
                    }
                    if (!ok) beep();
                    break;
            }
        }



        // Navigation Haut/Bas
        ui_handle_input(ch, &list, &selected, &offset);

        counter++;
    }

    free_process_list(&list);
    ui_close();
    return 0;
}
