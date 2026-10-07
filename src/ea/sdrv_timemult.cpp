/* A fragment of the sound driver (snddrv.c, 0x8016c1e8): the time
   multiplier and the low-pass cutoff (the value over half the output rate at
   +42; the constants are entries of the file's .sdata2 pool) of a mixer
   voice's channels. sndgs is named by its symbol; its
   view and the voice records are inferred. */
extern "C" char sndgs[];

/* inferred: a sound voice record (128 bytes) */
struct SNDVOICEVIEW {
    unsigned char unknown00[4];
    short channels[4];
    unsigned char unknown0c[19];
    unsigned char count;
    unsigned short flags;
    unsigned char unknown22[94];
};

extern "C" void MIX_settimemult(int, int);

extern "C" void MIX_setlowpass(int, float);

int SNDPLATFORM_timemult(int voice, int value) {
    SNDVOICEVIEW* sound = &(*(SNDVOICEVIEW**)(sndgs + 468))[voice];
    int i;

    if (sound->flags & 4) {
        for (i = 0; i < sound->count; i++)
            MIX_settimemult(sound->channels[i] - *(unsigned char*)(sndgs + 51), value);
    }
    return 0;
}

void SNDPLATFORM_lowpass(int voice, int value) {
    SNDVOICEVIEW* sound = &(*(SNDVOICEVIEW**)(sndgs + 468))[voice];
    int i;

    if (sound->flags & 4) {
        int cutoff = value * (1.0f / (0.5f * *(unsigned short*)(sndgs + 42)));
        for (i = 0; i < sound->count; i++)
            MIX_setlowpass(sound->channels[i] - *(unsigned char*)(sndgs + 51), cutoff);
    }
}
