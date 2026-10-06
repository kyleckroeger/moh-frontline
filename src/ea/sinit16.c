// MIXI_initunpack16: register the 16-bit PCM unpackers (init function and
// state size) with the mixer. sndmix's layout is not known; the slots are
// written through inferred offsets.
struct UNPACKINITPARAMS;
typedef void (*UNPACKINIT)(void*, UNPACKINITPARAMS*);

extern "C" char sndmix[];
void SFILTER_unpackfinit(void*, UNPACKINITPARAMS*);
void SFILTER_unpacklfinit(void*, UNPACKINITPARAMS*);
void SFILTER_unpackpfinit(void*, UNPACKINITPARAMS*);

extern "C" void MIXI_initunpack16(void) {
    *(UNPACKINIT*)(sndmix + 36) = SFILTER_unpackfinit;
    *(int*)(sndmix + 168) = 44;
    *(UNPACKINIT*)(sndmix + 40) = SFILTER_unpacklfinit;
    *(int*)(sndmix + 172) = 48;
    *(UNPACKINIT*)(sndmix + 44) = SFILTER_unpackpfinit;
    *(int*)(sndmix + 176) = 52;
}
