// MEM_totalunused: the free bytes left in a memory class (the default class
// when flags is 0). MEMCLASS is named by the mangled symbol; it is only used
// through a pointer here.
struct MEMCLASS;

extern MEMCLASS* memclass[64];

extern "C" {
extern int mb_default;
}
int FREE_gettotalfree(MEMCLASS*, int);

extern "C" int MEM_totalunused(int flags) {
    if (!flags)
        flags = mb_default;
    return FREE_gettotalfree(memclass[flags & 0x3f], 0);
}
