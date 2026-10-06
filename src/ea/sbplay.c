// SNDBANK_play: play a patch of a loaded bank. sndgs's layout is not known;
// the slot table is read through inferred offsets. BANKVER5, TAGGEDPATCH and
// SNDPLAYOPTS are named by the mangled symbols.
extern "C" char sndgs[];

struct TAGGEDPATCH;
struct SNDPLAYOPTS;

struct BANKVER5 {
    char field0[6];
    unsigned short patchcount;
};

struct SNDBANKSLOT {
    BANKVER5* header;
    int field4;
    int field8;
};

int SNDBANKI_valid(int);
TAGGEDPATCH* SNDBANKI_getppatch(BANKVER5*, int);
int SNDBANKI_playpatch(void*, TAGGEDPATCH*, int, int, SNDPLAYOPTS*);

extern "C" int SNDBANK_play(int bank, int patch, SNDPLAYOPTS* opts) {
    BANKVER5* header;

    if (SNDBANKI_valid(bank) < 0)
        return -8;
    header = (*(SNDBANKSLOT**)(sndgs + 472))[bank].header;
    if (patch < 0 || patch >= header->patchcount)
        return -8;
    return SNDBANKI_playpatch(header, SNDBANKI_getppatch(header, patch), bank, patch, opts);
}
