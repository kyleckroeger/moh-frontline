// SNDI_calcfxlevel: a voice's send level to an effect bus (bus level times the
// voice's two per-bus levels). sndgs's layout is not known; the voice record
// (128 bytes) is viewed through inferred members.
extern "C" char sndgs[];

struct SNDVOICEVIEW {
    char field0[32];
    unsigned short fxbus;
    char field22[50];
    signed char send[2];
    short fxlevel[2];
    char field5A[38];
};

signed char* SNDCTRLI_getfxbus(int, int);

void SNDI_calcfxlevel(int bus, int voice) {
    SNDVOICEVIEW* entry = &(*(SNDVOICEVIEW**)(sndgs + 468))[voice];
    signed char* fx = SNDCTRLI_getfxbus(bus, entry->fxbus);

    entry->fxlevel[bus] = fx[2] * (entry->send[bus] * entry->send[bus + 1]) >> 6;
}
