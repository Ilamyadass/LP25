#include "processus_ui.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h> 
#include <sys/types.h>

void ui_init() {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);

    start_color();
    init_pair(1, COLOR_GREEN,  COLOR_BLACK);
    init_pair(2, COLOR_CYAN,   COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_RED,    COLOR_BLACK); 
}

void ui_close() {
    endwin();
}

void ui_draw_header(Host host, int count) {
    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(0, 0, "MyTop - %s | %d processus", host.hostname, count);
    attroff(COLOR_PAIR(1) | A_BOLD);
    mvhline(1, 0, '-', COLS);
}

void ui_draw_footer() {
    int h, w;
    getmaxyx(stdscr, h, w);

    attron(COLOR_PAIR(2) | A_BOLD);
    mvprintw(h-2, 1,
        "F1 Aide | F2 Suivant | F3 Précédent | F5 Pause | F6 Stop | F7 Kill | F8 Reprendre | q Quitter");
    attroff(COLOR_PAIR(2) | A_BOLD);
}

void ui_draw_table(ProcessList *list, int selected, int offset) {
    int y = 2;

    attron(A_BOLD);
    mvprintw(y, 0,
        "%-6s %-10s %-8s %6s %6s %s",
        "PID", "USER", "TIME", "%CPU", "%MEM", "COMMAND");
    attroff(A_BOLD);

    y++;

    int max_lines = LINES - 4;

    for (int i = 0; i < max_lines && (i + offset) < list->count; i++) {

        ProcessInfo *p = &list->items[i + offset];

        if ((i + offset) == selected) {
            attron(A_REVERSE | COLOR_PAIR(3));
        }

        mvprintw(y + i, 0,
            "%-6d %-10s %-8s %6.1f %6.1f %.40s",
            p->pid, p->user, p->time, p->cpu, p->mem, p->cmd);

        if ((i + offset) == selected) {
            attroff(A_REVERSE | COLOR_PAIR(3));
        }
    }
}


int ui_process_action(int key, ProcessInfo *p) {
    // Si aucun processus n'est sélectionné (liste vide), on ne fait rien
    if (p == NULL) return 0;

    
    move(1, 0);
    clrtoeol(); 

    switch (key) {
        case KEY_F(5): // PAUSE
            if (kill(p->pid, SIGSTOP) == 0) {
                attron(COLOR_PAIR(3) | A_BOLD);
                mvprintw(1, 2, "[PAUSE] Signal SIGSTOP envoyé à %d (%s)", p->pid, p->cmd);
                attroff(COLOR_PAIR(3) | A_BOLD);
            } else {
                attron(COLOR_PAIR(4) | A_BOLD);
                mvprintw(1, 2, "Erreur: Impossible de mettre en pause %d (Droits insuffisants ?)", p->pid);
                attroff(COLOR_PAIR(4) | A_BOLD);
            }
            return 1;

        case KEY_F(6): // STOP (SIGTERM - arrêt propre)
            if (kill(p->pid, SIGTERM) == 0) {
                attron(COLOR_PAIR(4) | A_BOLD);
                mvprintw(1, 2, "[STOP] Signal SIGTERM envoyé à %d", p->pid);
                attroff(COLOR_PAIR(4) | A_BOLD);
            } else {
                mvprintw(1, 2, "Erreur: Impossible d'arrêter %d", p->pid);
            }
            return 2;

        case KEY_F(7): // KILL (SIGKILL - arrêt forcé)
            if (kill(p->pid, SIGKILL) == 0) {
                attron(COLOR_PAIR(4) | A_REVERSE | A_BOLD);
                mvprintw(1, 2, "[KILL] Signal SIGKILL envoyé à %d (RIP)", p->pid);
                attroff(COLOR_PAIR(4) | A_REVERSE | A_BOLD);
            } else {
                mvprintw(1, 2, "Erreur: Impossible de tuer %d", p->pid);
            }
            return 3;

        case KEY_F(8): // RESTART / CONTINUE
            if (kill(p->pid, SIGCONT) == 0) {
                attron(COLOR_PAIR(1) | A_BOLD);
                mvprintw(1, 2, "[CONT] Signal SIGCONT envoyé à %d", p->pid);
                attroff(COLOR_PAIR(1) | A_BOLD);
            } else {
                mvprintw(1, 2, "Erreur: Impossible de relancer %d", p->pid);
            }
            return 4;

        default:
            // Si aucune action, on remet la ligne de séparation
            mvhline(1, 0, '-', COLS);
            return 0;
    }
}

void ui_render(ProcessList *list, Host *hosts, int host_id, int selected, int offset) {
    clear();
    ui_draw_header(hosts[host_id], list->count);
    ui_draw_table(list, selected, offset);
    ui_draw_footer();
    refresh();
}


