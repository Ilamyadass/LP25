#include "ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <unistd.h>
#include <pwd.h>

#define FILE_SIZE 4096

int read_file(const char *path, char *const_file, size_t size) {
    FILE *f = fopen(path, "r");
    if (!f) return 0;
    size_t r = fread(const_file, 1, size - 1, f);
    const_file[r] = '\0';
    fclose(f);
    return 1;
}

float get_uptime() {
    char l_lu[FILE_SIZE];
    if (!read_file("/proc/uptime", l_lu, sizeof(l_lu))) return 0;
    return atof(l_lu);
}

void format_time(float seconds, char *out) {
    int minutes = (int)(seconds / 60);
    float sec_cs = seconds - (minutes * 60);
    sprintf(out, "%d:%05.2f", minutes, sec_cs);
}

float prev_total[65536];
float prev_uptime = 0;

int load_processes(ProcessList *list) {
    DIR *dir = opendir("/proc");
    if (!dir) return 0;

    float ticks = (float)sysconf(_SC_CLK_TCK);
    float page_size = (float)sysconf(_SC_PAGESIZE);
  
    float total_mem_kb = 1;
    {
        char l_lu[FILE_SIZE];
        FILE *f = fopen("/proc/meminfo", "r");
        if (f) {
            while (fgets(l_lu, sizeof(l_lu), f)) {
                sscanf(l_lu, "MemTotal: %f kB", &total_mem_kb);
            }
            fclose(f);
        }
    }

    float uptime = get_uptime();
    float delta_time = (prev_uptime > 0) ? (uptime - prev_uptime) : 1;

    ProcessInfo *items = malloc(sizeof(ProcessInfo) * 4096);
    int count = 0;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (!isdigit(entry->d_name[0]))
            continue;

        int pid = atoi(entry->d_name);

        char path[256], l_lu[FILE_SIZE];
        sprintf(path, "/proc/%d/stat", pid);
        if (!read_file(path, l_lu, sizeof(l_lu)))
            continue;

        char *p1 = strchr(l_lu,  '(');
        char *p2 = strrchr(l_lu, ')');
        if (!p1 || !p2) continue;

        char comm[256];
        int name_len = p2 - p1 - 1;
        if (name_len > 255) name_len = 255;
        memcpy(comm, p1 + 1, name_len);
        comm[name_len] = '\0';

        sscanf(l_lu, "%d", &pid);

        float utime, stime, starttime;
        char state;
        sscanf(p2 + 2,
               "%c "
               "%*d %*d %*d %*d %*d "
               "%*u %*u %*u %*u %*u "
               "%f %f "
               "%*d %*d %*d %*d %*d "
               "%f",
               &state, &utime, &stime, &starttime);

        float cpu_time = (utime + stime) / ticks;
        float total = utime + stime;

        float cpu = 0;
        if (prev_total[pid] > 0) {
            float diff = total - prev_total[pid];
            cpu = (diff / ticks) / delta_time * 100;
        }
        prev_total[pid] = total;

        // mémoire
        sprintf(path, "/proc/%d/statm", pid);
        float rss = 0;
        if (read_file(path, l_lu, sizeof(l_lu))) {
            float tmp;
            sscanf(l_lu, "%f %f", &tmp, &rss);
        }
        float mem = (rss * page_size / 1024) * 100 / total_mem_kb;

        // user
        sprintf(path, "/proc/%d/status", pid);
        int uid = -1;
        if (read_file(path, l_lu, sizeof(l_lu))) {
            char *up = strstr(l_lu, "Uid:");
            if (up) sscanf(up, "Uid:\t%d", &uid);
        }
        struct passwd *pw = getpwuid(uid);
        const char *user = pw ? pw->pw_name : "unknown";

        // stockage des valeurs
        ProcessInfo *P = &items[count++];
        P->pid = pid;
        strncpy(P->user, user, 31);
        strncpy(P->cmd, comm, 255);
        P->cpu = cpu;
        P->mem = mem;

        format_time(cpu_time, P->time);

        if (count >= 4096) break;
    }

    closedir(dir);

    // inversion du tableau pour avoir les derniers processus lancés
    for (int i = 0; i < count / 2; i++) {
        ProcessInfo tmp = items[i];
        items[i] = items[count - 1 - i];
        items[count - 1 - i] = tmp;
    }

    list->items = items;
    list->count = count;

    prev_uptime = uptime;

    return 1;
}
