// iSNDcalcvol: a voice's output volume from its sound, current and attribute
// volumes and the master volume (each out of 127), then through the voice's
// optional per-key scale and curve tables. sndgs's layout is not known; the
// voice record (128 bytes) is viewed through inferred members.
extern "C" char sndgs[];

struct SNDVOICEVIEW {
    char field0[52];
    int volume;
    char field38[4];
    int attrvolume;
    char field40[4];
    signed char soundvolume;
    signed char outvolume;
    char field46[21];
    unsigned char key;
    char field5C[8];
    signed char* curve;
    char field68[4];
    signed char* keyscale;
    char field70[16];
};

void iSNDcalcvol(int voice) {
    SNDVOICEVIEW* entry = &(*(SNDVOICEVIEW**)(sndgs + 468))[voice];

    entry->outvolume = *(signed char*)(sndgs + 361) * ((entry->attrvolume >> 16) * (entry->soundvolume * (entry->volume >> 16))) / 2048383;
    if (entry->keyscale)
        entry->outvolume = entry->outvolume * entry->keyscale[entry->key] / 127;
    if (entry->curve)
        entry->outvolume = entry->curve[entry->outvolume];
}
