// MEM_init: take the largest block the OS heap can give and make it the
// default memory class, freeing everything again at exit (MEM_restore).
// MEMCLASS is named by the mangled symbols; only the member at offset 88 is
// used here, and its name is not original.
struct MEMCLASS {
    char header[88];
    void* osblock;
};

extern MEMCLASS* memclass[64];

int iGC_GetMaxOsMemory(void);
void iGC_InitMem(void);

extern "C" {
extern volatile int __OSCurrHeap;
void* OSAllocFromHeap(int, unsigned long);
void REAL_addexit(void (*)(void));
void MEM_restore(void);
int MEM_initadr(void*, int);

int MEM_init(void) {
    int result;
    int size = iGC_GetMaxOsMemory();
    void* block;

    result = 0;
    REAL_addexit(MEM_restore);
    iGC_InitMem();
    block = OSAllocFromHeap(__OSCurrHeap, size);
    if (block) {
        result = MEM_initadr(block, size);
        memclass[0]->osblock = block;
    }
    return result;
}
}
