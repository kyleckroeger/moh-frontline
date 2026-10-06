// Sets up the main heap: the OS arena, less 24 MB when it is larger than that.
extern "C" {
void* OSGetArenaLo(void);
void* OSGetArenaHi(void);
void OSReport(const char*, ...);
int MEM_initadr(void*, int);

bool g_bMallocInitialized;

void mallocinit(void) {
    void* low = OSGetArenaLo();
    int size = (char*)OSGetArenaHi() - (char*)low;
    if (size > 0x1800000)
        size -= 0x1800000;
    int result = MEM_initadr(low, size);
    g_bMallocInitialized = true;
    OSReport("\n");
    OSReport("\n");
    OSReport("MEM_init() return %d\n", result);
    OSReport("\n");
}
}
