// MIXI_initunpackxa: register the XA ADPCM unpackers (init function and state
// size) with the mixer. sndmix's layout is not known; the slots are written
// through inferred offsets.
struct UNPACKINITPARAMS;
typedef void (*UNPACKINIT)(void*, UNPACKINITPARAMS*);

extern "C" char sndmix[];
void SFILTER_unpackxafinit(void*, UNPACKINITPARAMS*);
void SFILTER_unpackxalfinit(void*, UNPACKINITPARAMS*);
void SFILTER_unpackxapfinit(void*, UNPACKINITPARAMS*);

extern "C" void MIXI_initunpackxa(void) {
    *(UNPACKINIT*)(sndmix + 48) = SFILTER_unpackxafinit;
    *(int*)(sndmix + 180) = 44;
    *(int*)(sndmix + 184) = 68;
    *(UNPACKINIT*)(sndmix + 52) = SFILTER_unpackxalfinit;
    *(UNPACKINIT*)(sndmix + 56) = SFILTER_unpackxapfinit;
    *(int*)(sndmix + 188) = 56;
}
