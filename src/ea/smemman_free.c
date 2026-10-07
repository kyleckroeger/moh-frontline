/* A fragment of EA's sound memory manager (smemman.c, 0x80158ff0):
   SNDMEMI_free removes the allocation record of a block (records are kept
   downward from the end of the pool, the count as a negative number),
   moving the lower records up and returning the record's 8 bytes. The pool
   and record views, the inline record test and sndgs's view are inferred.
   SNDMEMI_allocz before it is drafted in scratch/game/smemman_full_wip.cpp. */
struct SNDMEMRECORDVIEW {
    unsigned int offset;
    int size;
};

/* inferred: the sound memory pool header */
struct SNDMEMPOOLVIEW {
    char* base;
    SNDMEMRECORDVIEW* records;
    int size;
    unsigned int free;
    int lowest;
    int count;
};

struct SNDGSVIEW {
    unsigned char unknown000[516];
    SNDMEMPOOLVIEW* pool;
};

extern SNDGSVIEW sndgs;

/* inferred: whether a record holds an offset */
static inline int recordat(SNDMEMRECORDVIEW* record, unsigned int offset) {
    return record->offset == offset;
}

void SNDMEMI_free(void* memory) {
    SNDMEMPOOLVIEW* pool = sndgs.pool;
    unsigned int offset = (char*)memory - pool->base;
    int i;

    for (i = 0; i > pool->count; i--) {
        if (recordat(&pool->records[i], offset)) {
            pool->count++;
            sndgs.pool->free += 8;
            for (; i > sndgs.pool->count; i--)
                sndgs.pool->records[i] = sndgs.pool->records[i - 1];
            return;
        }
    }
}
