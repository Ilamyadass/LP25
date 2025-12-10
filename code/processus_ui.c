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

//ça fonctionne mais le refresh est assez long 
int ui_process_action(int key, ProcessInfo *p) {
    if (p == NULL) return 0;

    switch (key) {
        case KEY_F(5): // PAUSE
            kill(p->pid, SIGSTOP);
            return 1;

        case KEY_F(6): // STOP (Demande d'arrêt)
            kill(p->pid, SIGTERM);
            return 2;

        case KEY_F(7): // KILL (Forcer l'arrêt)
            kill(p->pid, SIGKILL);
            return 3;

        case KEY_F(8): // RESTART / CONTINUE
            kill(p->pid, SIGCONT);
            return 4;

        default:
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


