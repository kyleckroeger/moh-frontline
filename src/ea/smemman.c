/* The start of EA's sound memory manager (smemman.c): the pool is laid out
   in caller memory with a table of (offset, size) allocation records at its
   end, indexed downward. SNDMEMI_init lays the pool out (data 16-byte
   aligned after a 24-byte header, the free and lowest-free sizes set) and
   SNDMEMI_restore asks for the high-water mark. SNDMEMI_allocz and
   SNDMEMI_free after them are not part of the unit (drafts in
   scratch/game/smemman_full_wip.cpp). The pool and record views and sndgs's
   are inferred. */
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

extern "C" int SNDMEM_gethighwater();

void SNDMEMI_init(void* memory, int size) {
    sndgs.pool = (SNDMEMPOOLVIEW*)memory;
    ((SNDMEMPOOLVIEW*)memory)->size = size;
    sndgs.pool->records = (SNDMEMRECORDVIEW*)((char*)memory + size - 8);
    sndgs.pool->base = (char*)memory + 24;
    sndgs.pool->base += 15;
    sndgs.pool->base = (char*)((int)sndgs.pool->base & ~15);
    sndgs.pool->free = size - 47;
    sndgs.pool->lowest = size - 15;
}

void SNDMEMI_restore() {
    SNDMEM_gethighwater();
}
