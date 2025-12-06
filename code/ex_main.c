#include "ui.h"

int main() {
    ui_init();

    Host hosts[1] = { {"local", 1} };

    ProcessInfo items[2] = {
        {1234, "root", 12.4, 1.1, "00:03:12", "sshd -D"},
        {2345, "user", 50.0, 3.4, "00:01:22", "python3 app.py"}
    };

    ProcessList list = { items, 2 };

    while (1) {
        ui_render(&list, hosts, 0);
        int key = getch();
        if (key == 'q')
            break;
    }

    ui_close();
    return 0;
}
