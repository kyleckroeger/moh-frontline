// Fragment of the ARAM sample-memory manager: set-up, pool bounds (in 32-byte
// units) and teardown. The allocator itself (SNDARAM_alloc/SNDARAM_free)
// follows in the original file and is not reconstructed. sndaram's members
// are inferred from offsets and are not original.
struct SNDARAMVIEW {
    unsigned short count;
    unsigned short max;
    unsigned int start;
    unsigned int end;
    void* table;
};

extern "C" {
extern SNDARAMVIEW sndaram;
void SNDSYS_entercritical(void);
void SNDSYS_leavecritical(void);
}
void* SNDMEMI_allocz(int);
void SNDMEMI_free(void*);

void SNDARAM_init(int blocks) {
    SNDSYS_entercritical();
    sndaram.table = SNDMEMI_allocz(blocks * 8);
    SNDSYS_leavecritical();
    sndaram.count = 0;
    sndaram.max = blocks;
}

void SNDARAM_setpool(unsigned int base, int size) {
    sndaram.end = (base + size) & ~4;
    if (base < 32)
        base = 32;
    sndaram.start = base;
    if (base & 31)
        sndaram.start = (base + 31) & ~31;
    sndaram.start >>= 5;
    sndaram.end >>= 5;
}

void SNDARAM_restore(void) {
    if (sndaram.table) {
        SNDSYS_entercritical();
        SNDMEMI_free(sndaram.table);
        SNDSYS_leavecritical();
        sndaram.table = 0;
    }
}
