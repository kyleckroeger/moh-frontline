// DWI allocation entry points and the global operator new/delete on the REAL
// memory manager. Every allocation is cleared; the operator new forms clear
// the block a second time after the inlined DWI_alloc.
extern "C" {
extern bool g_bMallocInitialized;
void mallocinit(void);
void* MEM_allocalign(const char*, int, int, int, int);
void* MEM_alloc(const char*, int, int);
void MEM_free(void*);
void* memset(void*, int, unsigned long);
}

void* DWI_allocalign(const char* name, int size, int align, int flags) {
    void* block;

    if (!g_bMallocInitialized)
        mallocinit();
    block = MEM_allocalign(name, size, align, 0, flags);
    memset(block, 0, size);
    return block;
}

void* DWI_alloc(const char* name, int size, int flags) {
    void* block;

    if (!g_bMallocInitialized)
        mallocinit();
    block = MEM_alloc(name, size, flags);
    memset(block, 0, size);
    return block;
}

void operator delete[](void* block) throw() {
    MEM_free(block);
}

void operator delete(void* block) throw() {
    MEM_free(block);
}

void* operator new[](unsigned long size, char* name) {
    void* block = DWI_alloc(name, size, 1024);

    memset(block, 0, size);
    return block;
}

void* operator new[](unsigned long size) {
    void* block = DWI_alloc("Default Debug Heap", size, 1024);

    memset(block, 0, size);
    return block;
}

void* operator new(unsigned long size) {
    void* block = DWI_alloc("Default Debug Heap", size, 1024);

    memset(block, 0, size);
    return block;
}
