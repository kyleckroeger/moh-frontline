// A fragment of the software mixer (smixer.c, 0x80160928): SNDMIX_setdrygain
// stores a mixer channel's dry gain for an output and marks the channel's
// gains for update. sndmix is named by its symbol; its view and the channel
// records (80 bytes) are inferred. SNDMIX_setwetgain after it uses a pooled
// float constant and is not part of this unit.
extern "C" char sndmix[];

/* inferred: a mixer channel's record */
struct MIXCHANNELVIEW {
    unsigned char unknown00;
    unsigned char dirty;
    unsigned char unknown02[18];
    float dry[5];
    float wet[5];
    unsigned char unknown3c[20];
};

extern "C" void SNDMIX_setdrygain(int channel, int output, float gain) {
    (*(MIXCHANNELVIEW**)(sndmix + 500))[channel].dry[output] = gain;
    (*(MIXCHANNELVIEW**)(sndmix + 500))[channel].dirty = 1;
}
