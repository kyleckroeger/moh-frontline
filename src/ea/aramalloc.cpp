// Fragment of the ARAM allocator: ARAM_NEW_poolmanager. ARAM_NEW_pool, which
// follows, differs only in register numbering (draft in
// scratch/tiny/aramalloc_full.cpp); the rest of the file is not reconstructed.
// The record layouts are inferred from offsets; the type and member names are
// not original (ARAM_POOL_MANAGER is named by the mangled symbols).
struct BPoolMan;
struct ARAM_POOL;

struct ARAM_POOL_MANAGER {
    ARAM_POOL* next;
    ARAM_POOL* prev;
    unsigned int align;
    int field0C;
    BPoolMan* nodes;
};

extern "C" {
extern int mb_default;
void* MEM_alloc(const char*, int, int);
BPoolMan* NEW_BPoolMan(unsigned int, unsigned int);
}

extern "C" ARAM_POOL_MANAGER* ARAM_NEW_poolmanager(unsigned int align, unsigned int maxnodes, int unused,
                                                   const char* name) {
    ARAM_POOL_MANAGER* manager = (ARAM_POOL_MANAGER*)MEM_alloc(name, 20, mb_default);

    manager->next = (ARAM_POOL*)manager;
    manager->prev = (ARAM_POOL*)manager;
    manager->align = align;
    manager->nodes = NEW_BPoolMan(maxnodes, 28);
    manager->field0C = 0;
    return manager;
}
