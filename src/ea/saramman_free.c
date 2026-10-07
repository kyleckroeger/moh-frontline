// Fragment of the ARAM sample-memory manager (saramman.c, 0x8016cf74):
// SNDARAM_free removes the allocation record of a block (its address in
// 32-byte units) and moves the later records down. sndaram's members and the
// record layout are inferred from offsets and are not original (see
// saramman.c). SNDARAM_alloc before it is not reconstructed.
/* inferred: an allocation record (start block and length) */
struct SNDARAMBLOCK {
    unsigned int start;
    unsigned int length;
};

struct SNDARAMVIEW {
    unsigned short count;
    unsigned short max;
    unsigned int start;
    unsigned int end;
    SNDARAMBLOCK* table;
};

extern "C" {
extern SNDARAMVIEW sndaram;
}

void SNDARAM_free(unsigned int address) {
    SNDARAMBLOCK* record = sndaram.table;
    unsigned int block = address >> 5;
    int i;

    for (i = 0; i < sndaram.count; record++, i++) {
        if (record->start == block) {
            sndaram.count--;
            for (; i < sndaram.count; i++)
                sndaram.table[i] = sndaram.table[i + 1];
            return;
        }
    }
}
