#ifndef UI_H
#define UI_H

#include <ncurses.h>

typedef struct {
    int pid;
    char user[32];
    float cpu;
    float mem;
    char time[32];
    char cmd[256];
} ProcessInfo;

typedef struct {
    ProcessInfo *items;
    int count;
} ProcessList;

typedef struct {
    char hostname[64];
    int connected;  // 1 = OK, 0 = erreur
} Host;

void ui_init();
void ui_close();
void ui_render(ProcessList *list, Host *hosts, int host_id);

void ui_draw_header(Host host);
void ui_draw_table(ProcessList *list);
void ui_draw_footer();

#endif
