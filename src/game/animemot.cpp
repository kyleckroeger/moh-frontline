// ARAM transfer callbacks: record the completion tick and clear the
// transfer-in-progress flag for the direction. The globals are named by the
// symbols; their types are inferred. The rest of the file is not part of this
// unit.
extern "C" unsigned long OSGetTick(void);

extern unsigned long end;
extern bool g_bDMAToARAM;
extern bool g_bDMAToMRAM;

void DMAToARAMCallback(unsigned long) {
    end = OSGetTick();
    g_bDMAToARAM = false;
}

void DMAFromARAMCallback(unsigned long) {
    end = OSGetTick();
    g_bDMAToMRAM = false;
}
