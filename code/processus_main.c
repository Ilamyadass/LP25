#include "processus_ui.h" // <--- Nouvelle ligne
#include <unistd.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>

int find_index_by_pid(ProcessList *list, int pid) {
    for (int i = 0; i < list->count; i++) {
        if (list->items[i].pid == pid) {
            return i;
        }
    }
    return 0;
}

int main() {
    ui_init();

    Host hosts[1];
    gethostname(hosts[0].hostname, sizeof(hosts[0].hostname));
    hosts[0].connected = 1;

    ProcessList list = {NULL, 0};
    int selected = 0;
    int offset = 0;

    int refresh_delay = 10;
    int counter = 0;

    while (1) {
        if (counter % refresh_delay == 0) {
            int selected_pid = 0;
            if (list.items && selected < list.count) {
                selected_pid = list.items[selected].pid;
            }

            free(list.items);
            load_processes(&list);
            
            if (selected_pid > 0) {
                selected = find_index_by_pid(&list, selected_pid);
                if (selected >= list.count) selected = list.count - 1;
            }
        }

        time_t now = time(NULL);
        char *tstr = ctime(&now);
        tstr[strlen(tstr)-1] = '\0'; // enlever le \n

        // Affiche liste
        ui_render(&list, hosts, 0, selected, offset);

        int ch = getch();
        if (ch == 'q') break;

        // Navigation vers le bas
        if (ch == KEY_DOWN) {
            if (selected < list.count - 1) {
                selected++;
                if (selected >= offset + (LINES - 4)) {
                    offset++;
                }
            }
        }

        // Navigation vers le haut
        if (ch == KEY_UP) {
            if (selected > 0) {
                selected--;
                if (selected < offset) {
                    offset--;
                }
            }
        }

       
        if (list.items && list.count > 0) {
            ui_process_action(ch, &list.items[selected]);
        }

      
        usleep(100000);
        counter++;
    }

    ui_close();
    free(list.items);
    return 0;
}
