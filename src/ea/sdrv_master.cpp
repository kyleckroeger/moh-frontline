/* A fragment of the sound driver (snddrv.c, 0x8016b600): a driver voice's
   master voice (itself when it has none; the master field is read through a
   byte offset into the voice table, as the original adds the field offset to
   the scaled index before the table base) and the channel a driver voice
   occupies within its master voice (1 to 3, or 0; an inline search). sndgs is
   named by its symbol; its view and the voice records are inferred. */
extern "C" char sndgs[];

/* inferred: a sound voice record (128 bytes) */
struct SNDVOICEVIEW {
    unsigned char unknown00[4];
    short channels[4];
    unsigned char unknown0c[24];
    short master;
    unsigned char unknown26[90];
};

int SNDDRV_getmastervoice(int voice) {
    int index = voice + *(unsigned char*)(sndgs + 51);
    int master = *(short*)(*(char**)(sndgs + 468) + index * 128 + 36);

    if (master == -1)
        master = index;
    return master;
}

/* inferred: the channel slot holding a driver voice */
static inline int channelof(SNDVOICEVIEW* master, int index) {
    int i;

    for (i = 1; i < 4; i++) {
        if (master->channels[i] == index)
            return i;
    }
    return 0;
}

int SNDDRV_getsamplechan(int voice) {
    SNDVOICEVIEW* voices = *(SNDVOICEVIEW**)(sndgs + 468);
    int index = voice + *(unsigned char*)(sndgs + 51);
    int master = voices[index].master;

    if (master == -1)
        return 0;
    return channelof(&voices[master], index);
}
