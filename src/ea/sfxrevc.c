/* MIX_restorereverb, the last function of the reverb set-up file: it clears
   the mixer's reverb state, restores the second-generation effects when they
   were initialised, frees the eleven filter-bank buffers and clears the
   reverb flag. sndmix, sndfb and SNDMIXfxv2inited are named by their
   symbols; the views of them (and of each 36-byte filter-bank entry) are
   inferred from offsets. */
void SNDMIXI_restorefx2();
void SNDMEMI_free(void*);

struct SNDMIXVIEW {
    unsigned char unknown00[15];
    unsigned char reverbOn;
    int reverb0;
    int reverb1;
    unsigned char unknown18[488];
};

struct SNDFBENTRYVIEW {
    unsigned char unknown00[12];
    void* buffer;
    unsigned char unknown10[20];
};

extern SNDMIXVIEW sndmix;
extern SNDFBENTRYVIEW sndfb[11];
extern int SNDMIXfxv2inited;

extern "C" void MIX_restorereverb() {
    int i;
    sndmix.reverb0 = 0;
    sndmix.reverb1 = 0;
    if (SNDMIXfxv2inited)
        SNDMIXI_restorefx2();
    for (i = 0; i < 11; i++) {
        if (sndfb[i].buffer) {
            SNDMEMI_free(sndfb[i].buffer);
            sndfb[i].buffer = 0;
        }
    }
    sndmix.reverbOn = 0;
}
