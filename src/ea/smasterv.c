// SNDmastervol: set the master volume and recompute every playing voice.
// sndgs's layout is not known; its fields are accessed through inferred
// offsets.
extern "C" {
extern char sndgs[];
void SNDSYS_entercritical(void);
void SNDSYS_leavecritical(void);
}
void iSNDcalcvol(int);
void SNDPLATFORM_setvol(int);

struct SNDVOICEVIEW {
    int handle;
    char field4[89];
    unsigned char state;
    char field5E[34];
};

extern "C" int SNDmastervol(int volume) {
    int i;

    SNDSYS_entercritical();
    *(unsigned char*)(sndgs + 361) = volume;
    for (i = 0; i < *(short*)(sndgs + 368); i++) {
        SNDVOICEVIEW* entry = &(*(SNDVOICEVIEW**)(sndgs + 468))[i];

        if ((int)entry->state == 1 && entry->handle >= 0) {
            iSNDcalcvol(i);
            SNDPLATFORM_setvol(i);
        }
    }
    SNDSYS_leavecritical();
    return 0;
}
