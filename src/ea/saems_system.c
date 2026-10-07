/* A fragment of saems.c (0x8015385c), the start of the file: the AEMS
   system's restore hook, SNDAEMSI_addsystem (installs a loaded system,
   registers the 100 Hz module timer and the restore hook, turns the
   system's offsets into pointers, and allocates the per-global state and the
   16 bank slots) and SNDAEMSI_removesystem (removes every bank and frees the
   state). The system layout, the trigger and parameter records and the
   sndaems and sndgs views are inferred. */
struct MODULEBANK;

/* inferred: a reference list (offset, then count) */
struct AEMSREFLISTVIEW {
    int* refs;
    short count;
};

/* inferred: a trigger record (parameter lists, then three trigger lists) */
struct AEMSTRIGGERVIEW {
    short paramCount;
    unsigned char unknown02[2];
    AEMSREFLISTVIEW* params;
    AEMSREFLISTVIEW lists[3];
};

/* inferred: a loaded AEMS system (counts, then tables stored as offsets) */
struct AEMSSYSTEMVIEW {
    unsigned char unknown00[12];
    short triggerCount;
    short globalCount;
    void* params;
    void* refs;
    AEMSTRIGGERVIEW* triggers;
    AEMSREFLISTVIEW* globals;
};

struct SNDAEMSVIEW {
    int unknown00;
    int rate;
    AEMSSYSTEMVIEW* system;
    MODULEBANK** banks;
    unsigned char unknown10[4];
    int* globalState;
    unsigned char unknown18[19];
    unsigned char handlerCount;
};

struct SNDGSVIEW {
    unsigned char unknown000[440];
    void (*aemsRestore)();
};

extern SNDAEMSVIEW sndaems;
extern SNDGSVIEW sndgs;
extern void (*SNDAEMS_removesystem)();
extern int (*SNDAEMS_removemodulebank)(int);

extern "C" {
void* memset(void*, int, unsigned long);
void SNDSYS_entercritical();
void SNDSYS_leavecritical();
void SNDSYS_add100hzclient(void (*)());
void SNDSYS_remove100hzclient(void (*)());
}
void SNDAEMS_linkbank();
void AEMSI_timerupdate();
void* SNDMEMI_allocz(int);
void SNDMEMI_free(void*);

void SNDAEMSI_restore() {
    SNDAEMS_removesystem();
}

int SNDAEMSI_addsystem(void* data) {
    AEMSSYSTEMVIEW* system = (AEMSSYSTEMVIEW*)data;
    int i;

    SNDAEMS_linkbank();
    SNDSYS_entercritical();
    sndaems.system = system;
    sndgs.aemsRestore = SNDAEMSI_restore;
    sndaems.unknown00 = 0;
    sndaems.rate = 10;
    SNDSYS_add100hzclient(AEMSI_timerupdate);
    sndaems.system = system;
    sndaems.handlerCount = 0;
    system->params = (void*)((int)system + (int)system->params);
    sndaems.system->refs = (void*)((int)system + (int)sndaems.system->refs);
    sndaems.system->triggers = (AEMSTRIGGERVIEW*)((int)system + (int)sndaems.system->triggers);
    sndaems.system->globals = (AEMSREFLISTVIEW*)((int)system + (int)sndaems.system->globals);
    for (i = 0; i < sndaems.system->triggerCount; i++) {
        AEMSTRIGGERVIEW* trigger = &sndaems.system->triggers[i];
        int j;

        trigger->params = (AEMSREFLISTVIEW*)((int)system + (int)trigger->params);
        for (j = 0; j < trigger->paramCount; j++)
            trigger->params[j].refs = (int*)((int)system + (int)trigger->params[j].refs);
        trigger->lists[0].refs = (int*)((int)system + (int)trigger->lists[0].refs);
        trigger->lists[1].refs = (int*)((int)system + (int)trigger->lists[1].refs);
        trigger->lists[2].refs = (int*)((int)system + (int)trigger->lists[2].refs);
    }
    for (i = 0; i < sndaems.system->globalCount; i++)
        sndaems.system->globals[i].refs = (int*)((int)system + (int)sndaems.system->globals[i].refs);
    if (sndaems.system->globalCount > 0) {
        sndaems.globalState = (int*)SNDMEMI_allocz(sndaems.system->globalCount * 4);
        memset(sndaems.globalState, 0, sndaems.system->globalCount * 4);
    }
    sndaems.banks = (MODULEBANK**)SNDMEMI_allocz(64);
    memset(sndaems.banks, 0, 64);
    SNDSYS_leavecritical();
    return 0;
}

int SNDAEMSI_removesystem() {
    int i;

    SNDSYS_entercritical();
    sndaems.handlerCount = 0;
    if (sndaems.system) {
        SNDSYS_remove100hzclient(AEMSI_timerupdate);
        for (i = 0; i < 16; i++) {
            if (sndaems.banks[i])
                SNDAEMS_removemodulebank(i);
        }
        SNDMEMI_free(sndaems.banks);
        sndaems.banks = 0;
        if (sndaems.system->globalCount > 0)
            SNDMEMI_free(sndaems.globalState);
        sndaems.system = 0;
        sndaems.globalState = 0;
    }
    SNDSYS_leavecritical();
    return 0;
}
