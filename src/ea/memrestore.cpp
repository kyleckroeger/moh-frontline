// MEM_restore (registered by MEM_init as an exit handler): clear duplicate
// memclass entries, then remove every class from 62 down to 0 and give the
// OS-heap block behind it back. MEMCLASS is named by the mangled symbols; only
// the member at offset 88 is used here, and its name is not original.
struct MEMCLASS {
    char header[88];
    void* osblock;
};

extern MEMCLASS* memclass[64];

extern "C" {
extern volatile int __OSCurrHeap;
void OSFreeToHeap(int, void*);
int MEMCLASS_remove(int);

void MEM_restore(void) {
    int i;
    int j;
    void* block;

    for (i = 0; i <= 63; i++) {
        for (j = 0; j < 64; j++) {
            if (i != j && memclass[i] == memclass[j] && memclass[j])
                memclass[j] = 0;
        }
    }
    for (i = 62; i >= 0; i--) {
        if (memclass[i]) {
            block = memclass[i]->osblock;
            MEMCLASS_remove(i);
            if (block)
                OSFreeToHeap(__OSCurrHeap, block);
            memclass[i] = 0;
        }
    }
}
}
