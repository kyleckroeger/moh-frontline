// A fragment of the software mixer (smixer.c, 0x80160928): SNDMIX_setdrygain
// stores a mixer channel's dry gain for an output and marks the channel's
// gains for update. sndmix is named by its symbol; its view and the channel
// records (80 bytes) are inferred. SNDMIX_setwetgain does the same for the
// wet gain, scaling it for outputs in mode 2 (the factor is an entry of the
// file's .sdata2 pool).
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

extern "C" void SNDMIX_setwetgain(int channel, int output, float gain) {
    if (sndmix[15 + output] == 2)
        (*(MIXCHANNELVIEW**)(sndmix + 500))[channel].wet[output] = 1.5f * gain;
    else
        (*(MIXCHANNELVIEW**)(sndmix + 500))[channel].wet[output] = gain;
    (*(MIXCHANNELVIEW**)(sndmix + 500))[channel].dirty = 1;
}
