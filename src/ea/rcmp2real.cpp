// RCMP_SYSTEM::SetREALDefaults: route RCMP's allocation through the REAL
// memory manager. Whether RCMP is a namespace or a class is not known; the
// member layout is inferred from offsets and its names are not original.
extern "C" {
void* MEM_allocalign(const char*, int, int, int, int);
int MEM_free(void*);
}

namespace RCMP {
struct RCMP_SYSTEM {
    int field0;
    void* (*allocate)(const char*, int, int, int, int);
    int (*release)(void*);
    void* fieldC;

    void SetREALDefaults();
};

void RCMP_SYSTEM::SetREALDefaults() {
    allocate = MEM_allocalign;
    release = MEM_free;
    fieldC = 0;
}
}
