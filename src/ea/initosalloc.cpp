// Sets up the OS heap for the REAL memory system and routes MSL's __sys_alloc
// to it. iGC_OsMemory is the memory reserved for the OS heap (8 KB).
extern "C" {
void* OSGetArenaLo(void);
void* OSGetArenaHi(void);
void* OSInitAlloc(void*, void*, int);
void OSSetArenaLo(void*);
int OSCreateHeap(void*, void*);
int OSSetCurrentHeap(int);
void MEM_init(void);
void* MEM_alloc(const char*, unsigned long, int);
int MEM_free(void*);
extern unsigned int memclass[64];

int iGC_OsMemory = 0x2000;
}

static int iGC_OsMemoryInitized;

int iGC_GetMaxOsMemory() {
    if (iGC_OsMemoryInitized == 0) {
        void* arenaLo = OSGetArenaLo();
        void* arenaHi = OSGetArenaHi();
        arenaLo = (void*)(((unsigned int)arenaLo + 31) & ~31);
        arenaHi = (void*)((unsigned int)arenaHi & ~31);
        return ((unsigned int)arenaHi - (unsigned int)arenaLo) - iGC_OsMemory;
    }
    return 0;
}

void iGC_InitMem() {
    if (iGC_OsMemoryInitized == 0) {
        iGC_OsMemoryInitized = 1;
        void* arenaLo = OSGetArenaLo();
        void* arenaHi = OSGetArenaHi();
        arenaLo = OSInitAlloc(arenaLo, arenaHi, 1);
        OSSetArenaLo(arenaLo);
        arenaLo = (void*)(((unsigned int)arenaLo + 31) & ~31);
        arenaHi = (void*)((unsigned int)arenaHi & ~31);
        OSSetCurrentHeap(OSCreateHeap(arenaLo, arenaHi));
        OSSetArenaLo(arenaLo = arenaHi);
    }
}

extern "C" void* __sys_alloc(unsigned long size) {
    if (memclass[0] == 0)
        MEM_init();
    return MEM_alloc("malloc pool", size, 0);
}

extern "C" void __sys_free(void* block) {
    if (memclass[0] == 0)
        MEM_init();
    MEM_free(block);
}
