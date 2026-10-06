// Synchronous system tasks: a table of 16 callbacks that SYNCTASK_run calls
// once their tick deadline has passed. The entry layout is inferred from the
// offsets; its type and member names are not original.
typedef int (*SyncTaskFunc)(int, int);

struct SyncTaskEntry {
    SyncTaskFunc func;
    int interval;
    int next;
    int running;
};

extern "C" {
extern volatile int libticks;
void MEM_fill(void*, int, int);
}

static SyncTaskEntry systemtasksubs[16];

extern "C" void SYNCTASK_init(void) {
    MEM_fill(systemtasksubs, 0, sizeof(systemtasksubs));
}

extern "C" void SYNCTASK_add(SyncTaskFunc func, int interval, int delay) {
    int skip;
    int i;
    int slot = -1;
    static int reentry = 0;

    if (interval == -1)
        interval = 0;
    else if (interval == 0)
        interval = 1;
    skip = reentry;
    reentry++;
    for (i = 0; i < 16; i++) {
        if (systemtasksubs[i].func == func) {
            slot = i;
        } else if (systemtasksubs[i].func == 0 && slot == -1) {
            if (skip)
                skip--;
            else
                slot = i;
        }
    }
    if (slot != -1) {
        systemtasksubs[slot].func = func;
        systemtasksubs[slot].interval = interval;
        systemtasksubs[slot].next = libticks + delay;
        systemtasksubs[slot].running = 0;
    }
    reentry--;
}

extern "C" void SYNCTASK_del(SyncTaskFunc func) {
    int i;

    for (i = 0; i < 16 && systemtasksubs[i].func != func; i++)
        ;
    if (i < 16 && systemtasksubs[i].func == func)
        systemtasksubs[i].func = 0;
}

extern "C" int SYNCTASK_run(int arg) {
    static int lastsystemtask = 0;
    int i;
    int result = 0;

    lastsystemtask = libticks;
    for (i = 0; i < 16; i++) {
        SyncTaskFunc func = systemtasksubs[i].func;
        if (func && libticks >= systemtasksubs[i].next && !systemtasksubs[i].running) {
            systemtasksubs[i].running = 1;
            result |= func(arg, libticks - systemtasksubs[i].next);
            systemtasksubs[i].next = libticks + systemtasksubs[i].interval;
            systemtasksubs[i].running = 0;
        }
    }
    return result;
}
