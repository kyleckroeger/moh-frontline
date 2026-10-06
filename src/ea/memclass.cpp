// REAL memory classes. MEMCLASS_create lays out a class in a block of memory: the LOW
// block holds the class record, one free block covers the rest, and an empty
// HIGH block ends the range.
//
// MEMBLOCK and MEMCLASS are named by the mangled symbols; their members are
// inferred from offsets (MEM_initblock stores magic, flags, size, next and
// prev; the free list links follow) and their names are not original.
struct MEMBLOCK {
    unsigned short magic;
    unsigned short flags;
    int size;
    MEMBLOCK* next;
    MEMBLOCK* prev;
    MEMBLOCK* freenext;
    MEMBLOCK* freeprev;
};

struct MEMCLASS {
    char name[8];
    MEMBLOCK* low;
    MEMBLOCK* high;
    MEMBLOCK free;
    int field28; // MEMCLASS_create's fifth argument; MEM_free (not reconstructed) rounds sizes up to it
    int alignment;
    int field30;
    int flags;
    unsigned char locked;
    int mutex[8];
};

static const int MB_SENTINEL = 0x2000;
static const int MB_NAME = 0x1000;

extern MEMCLASS* memclass[64];

extern "C" int sprintf(char*, const char*, ...);
extern "C" char* strcpy(char*, const char*);
extern "C" void MEM_fill(void*, int, int);
extern "C" void MUTEX_create(void*);
extern "C" void MUTEX_destroy(void*);
int MEM_initblock(MEMBLOCK*, const char*, int, int, int, MEMBLOCK*, MEMBLOCK*);
void FREE_add(MEMCLASS*, MEMBLOCK*);

extern "C" int MEMCLASS_create(int index, const char* name, void* address, int size, int arg4, int alignment,
                               int arg6, bool sentinel, bool named, bool locked) {
    char buffer[256];
    MEMBLOCK* low = (MEMBLOCK*)address;
    MEMBLOCK* block;
    MEMBLOCK* high;
    MEMCLASS* cls;
    int flags = index;

    if (sentinel)
        flags |= MB_SENTINEL;
    if (named)
        flags |= MB_NAME;
    block = (MEMBLOCK*)(((unsigned int)((char*)low + arg6 + 156) + (alignment - 1) & (unsigned int)~(alignment - 1)) - 16);
    high = (MEMBLOCK*)((int)low + size - 48 - arg6);
    cls = (MEMCLASS*)((char*)low + 16);
    sprintf(buffer, "%s LOW", name);
    MEM_initblock(low, buffer, 92, arg6, flags | 0x8000, 0, block);
    MEM_initblock(block, 0, (int)high - (int)block - 16, arg6, flags, low, high);
    sprintf(buffer, "%s HIGH", name);
    MEM_initblock(high, buffer, 0, arg6, flags | 0x8100, block, 0);
    memclass[index & 0x3f] = cls;
    MEM_fill(cls, 0, 92);
    strcpy(cls->name, name);
    cls->low = low;
    cls->high = high;
    cls->low->magic = 0x4253;
    cls->high->magic = 0x4253;
    cls->free.magic = 0x4253;
    cls->free.freenext = &cls->free;
    cls->free.freeprev = &cls->free;
    cls->free.size = 0x7fffffff;
    cls->field28 = arg4;
    cls->alignment = alignment;
    cls->field30 = arg6;
    cls->flags = flags;
    cls->locked = 0;
    FREE_add(cls, block);
    if (locked) {
        MUTEX_create(cls->mutex);
        cls->locked = 1;
    }
    return block->size;
}

extern "C" int MEMCLASS_remove(int index) {
    MEMCLASS* cls = memclass[index & 0x3f];
    int result = 0;

    if (cls) {
        if (cls->locked)
            MUTEX_destroy(cls->mutex);
        MEM_fill(cls, 0, 92);
        memclass[index & 0x3f] = 0;
        result = 1;
    }
    return result;
}
