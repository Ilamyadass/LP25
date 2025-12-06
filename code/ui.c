#include "ui.h"
#include <stdio.h>

void ui_init() {
    initscr();
    noecho();
    cbreak();
    curs_set(0);
    keypad(stdscr, TRUE);
    start_color();

    init_pair(1, COLOR_GREEN, COLOR_BLACK); // connect ok
    init_pair(2, COLOR_RED, COLOR_BLACK);   // connect fail
    init_pair(3, COLOR_CYAN, COLOR_BLACK);  // header
}

void ui_close() {
    endwin();
}

void ui_draw_header(Host host) {
    attron(COLOR_PAIR(3) | A_BOLD);
    mvprintw(0, 0, " Machine: %s ", host.hostname);
    attroff(A_BOLD);

    if (host.connected)
        attron(COLOR_PAIR(1));
    else
        attron(COLOR_PAIR(2));

    mvprintw(0, 20, "[Connexion: %s]", host.connected ? "OK" : "ERREUR");
    attroff(COLOR_PAIR(1) | COLOR_PAIR(2));

    mvhline(1, 0, '-', COLS);
}

void ui_draw_table(ProcessList *list) {
    mvprintw(2, 0, "PID     USER        CPU   MEM   TIME       CMD");
    mvhline(3, 0, '-', COLS);

    for (int i = 0; i < list->count && i < LINES - 5; i++) {
        ProcessInfo *p = &list->items[i];
        mvprintw(4 + i, 0,
            "%-7d %-10s %-5.1f %-5.1f %-10s %.50s",
            p->pid,
            p->user,
            p->cpu,
            p->mem,
            p->time,
            p->cmd
        );
    }
}

void ui_draw_footer() {
    mvhline(LINES - 2, 0, '-', COLS);
    mvprintw(LINES - 1, 0,
        "F1 Aide | F2 Suivant | F3 Précédent | F4 Rechercher | F5 Pause | F6 Stop | F7 Kill | F8 Restart");
}

void ui_render(ProcessList *list, Host *hosts, int host_id) {
    clear();
    ui_draw_header(hosts[host_id]);
    ui_draw_table(list);
    ui_draw_footer();
    refresh();
}

