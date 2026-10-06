// SNDBANK_patchinfo: describe a bank patch's sample format, attributes and
// data, dropping the per-channel buffers that the header conversion
// allocates. sndgs's layout is not known; the slot table and the attribute
// record are accessed through inferred offsets. The struct names come from
// the mangled symbols.
extern "C" char sndgs[];

struct TAGGEDPATCH;
struct SNDSAMPLEDESC;

struct SNDSAMPLEFORMAT {
    char field0[2];
    unsigned char channels;
};

struct SNDSAMPLEATTR {
    char field0[20];
    void* buffer[4];
    int size[4];
};

struct BANKVER5;

struct SNDBANKSLOT {
    BANKVER5* header;
    int field4;
    int field8;
};

extern "C" {
void SNDSYS_entercritical(void);
void SNDSYS_leavecritical(void);
}
int SNDBANKI_valid(int);
TAGGEDPATCH* SNDBANKI_getppatch(BANKVER5*, int);
void SNDI_patchtohdr(void*, TAGGEDPATCH*, SNDSAMPLEFORMAT*, SNDSAMPLEATTR*, SNDSAMPLEDESC*);
void SNDMEMI_free(void*);

extern "C" int SNDBANK_patchinfo(int bank, int patch, SNDSAMPLEFORMAT* format, SNDSAMPLEATTR* attr,
                                 SNDSAMPLEDESC* desc) {
    TAGGEDPATCH* tagged;
    int i;

    if (SNDBANKI_valid(bank) < 0)
        return -8;
    tagged = SNDBANKI_getppatch((*(SNDBANKSLOT**)(sndgs + 472))[bank].header, patch);
    if (!tagged)
        return -8;
    SNDSYS_entercritical();
    SNDI_patchtohdr((*(SNDBANKSLOT**)(sndgs + 472))[bank].header, tagged, format, attr, desc);
    for (i = 0; i < format->channels; i++) {
        if (attr->buffer[i]) {
            SNDMEMI_free(attr->buffer[i]);
            attr->buffer[i] = 0;
            attr->size[i] = 0;
        }
    }
    SNDSYS_leavecritical();
    return 0;
}
