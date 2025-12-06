#include "ui.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void ui_init() {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);

    start_color();
    init_pair(1, COLOR_GREEN,  COLOR_BLACK);  // Header
    init_pair(2, COLOR_CYAN,   COLOR_BLACK);  // Footer
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);  // Selected process
}

void ui_close() {
    endwin();
}

void ui_draw_header(Host host) {
    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(0, 0, " Gestionnaire de Processus — Machine : %s ", host.hostname);
    attroff(COLOR_PAIR(1) | A_BOLD);
    mvhline(1, 0, '-', COLS);
}

void ui_draw_footer() {
    int h, w;
    getmaxyx(stdscr, h, w);

    attron(COLOR_PAIR(2) | A_BOLD);
    mvprintw(h-2, 1, 
        "F1 Aide  |  F2 Suivant  |  F3 Précédent  |  F4 Rechercher | F5 Pause |  F6 Stop  |  F7 Kill |  F8 Redémarrer");
    attroff(COLOR_PAIR(2) | A_BOLD);
}

void ui_draw_table(ProcessList *list, int selected) {
    int y = 2; // juste après header
    attron(A_BOLD);
    mvprintw(y, 0, "PID     USER        CPU   MEM   TIME       CMD");
    attroff(A_BOLD);
    y++;

    for (int i = 0; i < list->count && y + i < LINES - 3; i++) {
        ProcessInfo *p = &list->items[i];
        if (i == selected) {
            attron(A_REVERSE | COLOR_PAIR(3)); // surbrillance
        }

        mvprintw(y + i, 0,
            "%-7d %-10s %-5.1f %-5.1f %-10s %.50s",
            p->pid,
            p->user,
            p->cpu,
            p->mem,
            p->time,
            p->cmd
        );

        if (i == selected) {
            attroff(A_REVERSE | COLOR_PAIR(3));
        }
    }
}

int ui_process_action(int key, ProcessInfo *p) {
    switch (key) {
        case KEY_F(5):
            mvprintw(1, COLS-30, "[PAUSE] Processus %d", p->pid);
            return 1;
        case KEY_F(6):
            mvprintw(1, COLS-30, "[STOP]  Processus %d", p->pid);
            return 2;
        case KEY_F(7):
            mvprintw(1, COLS-30, "[KILL]  Processus %d", p->pid);
            return 3;
        case KEY_F(8):
            mvprintw(1, COLS-30, "[RESTART] Processus %d", p->pid);
            return 4;
        default:
            return 0;
    }
}

void ui_render(ProcessList *list, Host *hosts, int host_id, int selected) {
    clear();
    ui_draw_header(hosts[host_id]);
    ui_draw_table(list, selected); // table draw utilise selected
    ui_draw_footer();
    refresh();
}


