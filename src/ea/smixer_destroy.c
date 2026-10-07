// A fragment of the software mixer (smixer.c, 0x801604b8): MIX_destroy frees
// the mixer's channel records, its two work buffers and the per-output mix
// buffers, clearing each pointer. sndmix is named by its symbol; its view is
// inferred.
/* inferred: the software mixer's state, as far as it is used here */
struct SNDMIXVIEW {
    unsigned char unknown000[5];
    unsigned char outputs;
    unsigned char unknown006[282];
    void* work[2];
    unsigned char unknown128[8];
    void* mix[49];
    void* channels;
};

extern "C" {
extern SNDMIXVIEW sndmix;
void SNDSYS_entercritical(void);
void SNDSYS_leavecritical(void);
}
void SNDMEMI_free(void*);

extern "C" void MIX_destroy(void) {
    int i;

    SNDSYS_entercritical();
    if (sndmix.channels) {
        SNDMEMI_free(sndmix.channels);
        sndmix.channels = 0;
    }
    for (i = 0; i < 2; i++) {
        if (sndmix.work[i]) {
            SNDMEMI_free(sndmix.work[i]);
            sndmix.work[i] = 0;
        }
    }
    for (i = 0; i < sndmix.outputs; i++) {
        if (sndmix.mix[i]) {
            SNDMEMI_free(sndmix.mix[i]);
            sndmix.mix[i] = 0;
        }
    }
    SNDSYS_leavecritical();
}
