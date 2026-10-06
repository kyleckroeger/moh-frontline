// GameCube side of the EA system module: critical sections and start-up.
// The critical section is a 28-byte MUTEX followed by its owner thread and a
// count; the global semaphore exists while any critical section does.
struct OSThread;
struct SysInitParms_t;

struct MUTEXView {
    unsigned char unknown00[28];
};

struct SysCriticalSectionView {
    MUTEXView mutex;
    OSThread* owner;
    int count;
};

extern "C" {
void MUTEX_create(MUTEXView*);
void MUTEX_destroy(MUTEXView*);
OSThread* OSGetCurrentThread(void);
void OSInit(void);
void DVDInit(void);
}

static MUTEXView _Sys_CritSectSema;
static int _Sys_CritSectCtr;

extern "C" void SysShutdownCriticalSectionFunc(SysCriticalSectionView* section) {
    MUTEX_destroy(&section->mutex);
    section->owner = 0;
    if (--_Sys_CritSectCtr == 0)
        MUTEX_destroy(&_Sys_CritSectSema);
}

int SysShutdownDependent() {
    return 0;
}

void SysLoadAllModules(bool, int) {
}

extern "C" void SysInitCriticalSectionFunc(SysCriticalSectionView* section) {
    if (_Sys_CritSectCtr == 0)
        MUTEX_create(&_Sys_CritSectSema);
    _Sys_CritSectCtr++;
    MUTEX_create(&section->mutex);
    section->owner = OSGetCurrentThread();
    section->count = 0;
}

int SysInitDependent(const SysInitParms_t*) {
    OSInit();
    DVDInit();
    return 0;
}
