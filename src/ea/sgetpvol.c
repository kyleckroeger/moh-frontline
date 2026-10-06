// SNDCTRL_getprogvol: a voice's program volume (-8 when the handle has no
// voice). sndgs's layout is not known; the voice table (128-byte entries) is
// read through inferred offsets.
extern "C" char sndgs[];
int SNDVOICEI_get(int);

struct SNDVOICEVIEW {
    char field0[52];
    int progvol;
    char field38[72];
};

extern "C" int SNDCTRL_getprogvol(int handle) {
    int voice = SNDVOICEI_get(handle);

    if (voice < 0)
        return -8;
    return (*(SNDVOICEVIEW**)(sndgs + 468))[voice].progvol >> 16;
}
