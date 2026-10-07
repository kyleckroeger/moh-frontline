// A fragment of the software mixer (smixer.c, 0x80160590): MIX_reset
// installs the default mixing and 16-bit decoding routines and the default
// frame length (320 samples). sndmix is named by its symbol; its view is
// inferred.
extern "C" {
extern char sndmix[];
void mixc(void);
void decode16x87(void);
}

extern "C" void MIX_reset(void) {
    *(void (**)(void))(sndmix + 504) = mixc;
    *(void (**)(void))(sndmix + 508) = decode16x87;
    *(short*)(sndmix + 12) = 320;
}
