#include "processus_ui.h"
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <ncurses.h>

void ui_init() {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE); // Active les touches F1, F2, Flèches...
    curs_set(0);          // Cache le curseur clignotant

    start_color();
    // Paires de couleurs : Texte, Fond
    init_pair(1, COLOR_GREEN,  COLOR_BLACK); // Titre connecté
    init_pair(2, COLOR_RED,    COLOR_BLACK); // Titre erreur
    init_pair(3, COLOR_CYAN,   COLOR_BLACK); // Menu du bas
    init_pair(4, COLOR_BLACK,  COLOR_CYAN);  // Ligne sélectionnée
}

void ui_close() {
    endwin();
}

// Affiche le haut (Header)
void ui_draw_header(Host *host, int count) {
    if (host->connected) {
        attron(COLOR_PAIR(1) | A_BOLD);
        mvprintw(0, 0, "[ %s ] (%s) - %d processus", host->name, host->hostname, count);
        attroff(COLOR_PAIR(1) | A_BOLD);
    } else {
        attron(COLOR_PAIR(2) | A_BOLD);
        mvprintw(0, 0, "[ %s ] ERREUR: %s", host->name, host->error_msg);
        attroff(COLOR_PAIR(2) | A_BOLD);
    }
    mvhline(1, 0, '-', COLS); // Ligne horizontale
}

// Affiche le tableau (Table)
void ui_draw_table(ProcessList *list, int selected, int offset) {
    // Titres des colonnes
    attron(A_BOLD);
    mvprintw(2, 0, "%-6s %-10s %-25s %6s %6s %s", "PID", "USER", "COMMAND", "%CPU", "%MEM", "TIME");
    attroff(A_BOLD);

    int max_y = LINES - 4; // Espace disponible pour la liste
    
    for (int i = 0; i < max_y && (i + offset) < list->count; i++) {
        int idx = i + offset;
        ProcessInfo *p = &list->items[idx];

        // Surbrillance si c'est la ligne sélectionnée
        if (idx == selected) {
            attron(COLOR_PAIR(4));
            // On efface toute la ligne pour que la couleur de fond aille jusqu'au bout
            move(3 + i, 0);
            clrtoeol(); 
        }

        // Affichage des données
        mvprintw(3 + i, 0, "%-6d %-10s %-25.25s %6.1f %6.1f %s", 
                 p->pid, p->user, p->cmd, p->cpu, p->mem, p->time);

        if (idx == selected) attroff(COLOR_PAIR(4));
    }
}

void ui_render(ProcessList *list, Host *host, int selected, int offset) {
    clear(); 

    ui_draw_header(host, list->count);
    ui_draw_table(list, selected, offset);

    // --- FOOTER ---
    int max_h, max_w;
    getmaxyx(stdscr, max_h, max_w);
    (void)max_w; // <--- AJOUTEZ CETTE LIGNE pour supprimer le warning
    
    attron(COLOR_PAIR(3));
    
    // Ligne de navigation
    mvprintw(max_h - 2, 0, "F2: Précédent | F3: Suivant | q: Quitter");

    // Ligne d'actions
    move(max_h - 1, 0);
    clrtoeol(); 
    
    if (host->type == CON_LOCAL) {
        printw("ACTIONS : F5 Pause | F6 Stop | F7 Kill | F8 Reprendre");
    } 
    else {
        attron(A_BOLD);
        printw("( Mode Lecture Seule - Actions désactivées à distance )");
        attroff(A_BOLD);
    }
    
    attroff(COLOR_PAIR(3));

    refresh();
}
// Gère la navigation Haut/Bas
int ui_handle_input(int key, ProcessList *list, int *selected, int *offset) {
    int max_lines = LINES - 4;

    switch (key) {
        case KEY_DOWN:
            if (*selected < list->count - 1) {
                (*selected)++;
                // Si on descend plus bas que l'écran, on décalle l'affichage (scroll)
                if (*selected >= *offset + max_lines) {
                    (*offset)++;
                }
            }
            break;

        case KEY_UP:
            if (*selected > 0) {
                (*selected)--;
                // Si on monte plus haut que l'écran, on remonte l'affichage
                if (*selected < *offset) {
                    (*offset)--;
                }
            }
            break;
    }
    return 0; 
}


// Gère les actions F5, F6, F7, F8 sur un processus
int ui_process_action(int key, ProcessInfo *p) {
    if (p == NULL) return 0;

    switch (key) {
        case KEY_F(5): // PAUSE
            kill(p->pid, SIGSTOP);
            return 1;

        case KEY_F(6): // STOP (Demande d'arrêt gentille)
            kill(p->pid, SIGTERM);
            return 2;

        case KEY_F(7): // KILL (Arrêt forcé)
            kill(p->pid, SIGKILL);
            return 3;

        case KEY_F(8): // RESTART / CONTINUE
            kill(p->pid, SIGCONT);
            return 4;

        default:
            return 0;
    }
}