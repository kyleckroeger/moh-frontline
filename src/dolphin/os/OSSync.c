#include <dolphin.h>
#include <dolphin/os.h>

#include "__os.h"

// prototypes
void __OSSystemCallVectorStart(void);
void __OSSystemCallVectorEnd(void);

void __OSInitSystemCall(void) {
    void* addr = (void*)OSPhysicalToCached(0xC00);

    memcpy(addr, __OSSystemCallVectorStart, (u32)&__OSSystemCallVectorEnd - (u32)&__OSSystemCallVectorStart);
    DCFlushRangeNoSync(addr, 0x100);
    __sync();
    ICInvalidateRange(addr, 0x100);
}
