// Hooks the sound system into the REAL runtime: a system task that services
// the sound system, an abort-message hook and an exit handler.
// Only the abort-message hook at sndgs +0xc4 is established.
struct SNDGSView {
    unsigned char unknown00[196];
    void (*abortmsg)(char*);
};

extern SNDGSView sndgs;

extern "C" {
void SNDSYS_service(void);
void SYNCTASK_add(int (*)(int, int), int, int);
void REAL_addexit(void (*)(void));
}
void SNDREAL_exithandler();

int SNDREAL_systemtask(int, int) {
    SNDSYS_service();
    return 0;
}

void SNDREAL_abortmsg(char*) {
}

extern "C" int SNDSYS_vectortoreal(void) {
    static int systaskadded = 0;

    sndgs.abortmsg = SNDREAL_abortmsg;
    if (!systaskadded) {
        SYNCTASK_add(SNDREAL_systemtask, 0, 1);
        systaskadded = 1;
    }
    REAL_addexit(SNDREAL_exithandler);
    return 0;
}
