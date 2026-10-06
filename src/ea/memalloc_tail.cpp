// A fragment of EA's memalloc.cpp (0x8014f4f8): MEM_tailsize, the tail size of
// the memory class selected by the low six flag bits (the MEMCLASS word at
// +0x30). The file name is this project's; the original record is memalloc.cpp
// (memalloc.cpp holds MEM_allocalign onward). MEMCLASS is the inferred view of
// memclass.cpp; the file's globals are extern.
// REAL memory allocation from a memory class. The block found on the free list
// is aligned (from the bottom, or from the top with flag 0x100) and any large
// enough remainder goes back on the free list as a new block.
//
// MEMBLOCK and MEMCLASS are named by the mangled symbols; their members are
// inferred from offsets (see memclass.cpp) and their names are not original.
struct MEMBLOCK {
    unsigned short magic;
    unsigned short flags;
    int size;
    MEMBLOCK* next;
    MEMBLOCK* prev;
};

struct MEMCLASS {
    char name[8];
    MEMBLOCK* low;
    MEMBLOCK* high;
    MEMBLOCK free;
    MEMBLOCK* field20;
    MEMBLOCK* field24;
    int field28;
    int alignment;
    int field30;
    int flags;
    unsigned char locked;
    int mutex[8];
};

extern MEMCLASS* memclass[64];

extern "C" {
extern int mb_default;
}
int (*MEM_allocfailcallback)(int, int, int);
extern "C" {
void MUTEX_lock(void*);
void MUTEX_unlock(void*);
}
int MEM_initblock(MEMBLOCK*, const char*, int, int, int, MEMBLOCK*, MEMBLOCK*);
MEMBLOCK* FREE_find(MEMCLASS*, int, int);
MEMBLOCK* FREE_findlargest(MEMCLASS*, int, int);
void FREE_remove(MEMBLOCK*);
void FREE_add(MEMCLASS*, MEMBLOCK*);

int MEM_tailsize(const char* name, int flags) {
    return memclass[flags & 0x3f]->field30;
}

static void* MEM_allocaligna(const char* name, int size, int align, int offset, int flags, bool clear);

extern "C" void* MEM_allocalign(const char* name, int size, int align, int offset, int flags);

extern "C" void* MEM_alloc(const char* name, int size, int flags);

extern "C" void* MEM_allocz(const char* name, int size, int flags);


