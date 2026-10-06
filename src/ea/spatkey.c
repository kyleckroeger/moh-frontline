// iSNDpatchkey: step *key through the platform voices that play the same
// patch as a voice (or yield the voice itself once when it has no patch).
// sndgs's layout is not known; the voice table (128-byte records) is viewed
// through inferred members.
extern "C" char sndgs[];

struct SNDVOICEVIEW {
    int handle;
    char field4[30];
    unsigned char patch;
    char field23[58];
    unsigned char state;
    char field5E[34];
};

int iSNDpatchkey(int voice, int* key) {
    int patch = (*(SNDVOICEVIEW**)(sndgs + 468))[voice].patch;

    if (patch != 0) {
        for (;;) {
            SNDVOICEVIEW* entry;

            (*key)++;
            if (*key >= *(short*)(sndgs + 368))
                break;
            entry = &(*(SNDVOICEVIEW**)(sndgs + 468))[*key];
            if (entry->patch == patch && (int)entry->state == 1 && entry->handle >= 0)
                return 1;
        }
    } else if (*key < 0) {
        *key = voice;
        return 1;
    }
    return 0;
}
