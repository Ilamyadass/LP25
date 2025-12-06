#include "ui.h"
#include <unistd.h>
#include <string.h>

int main() {
    ui_init();

    Host hosts[1] = { {"localhost", 1} };

    ProcessInfo items[5] = {
        {1234, "root",   12.0, 1.1, "00:03:21", "sshd"},
        {2345, "user",   30.5, 5.0, "00:01:10", "python3 app.py"},
        {3456, "user",   5.0,  2.0, "00:02:05", "bash"},
        {4567, "root",   0.5,  0.1, "00:10:00", "nginx"},
        {5678, "user",   8.0,  3.2, "00:00:45", "htop"}
    };

    ProcessList list = { items, 5 };
    int selected = 0;

    while (1) {
        ui_render(&list, hosts, 0, selected);
        int ch = getch();

        if (ch == 'q') break;
        if (ch == KEY_UP && selected > 0) selected--;
        if (ch == KEY_DOWN && selected < list.count - 1) selected++;

        ui_process_action(ch, &list.items[selected]);
    }

    ui_close();
    return 0;
}
