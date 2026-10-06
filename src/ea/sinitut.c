// MIXI_initunpackmt: register the MT unpackers (init function and state size)
// with the mixer. sndmix's layout is not known; the slots are written through
// inferred offsets.
struct UNPACKINITPARAMS;

extern "C" char sndmix[];
void SFILTER_unpackmtfinit(void*, UNPACKINITPARAMS*);
void SFILTER_unpackmtpfinit(void*, UNPACKINITPARAMS*);

extern "C" void MIXI_initunpackmt(void) {
    *(void (**)(void*, UNPACKINITPARAMS*))(sndmix + 60) = SFILTER_unpackmtfinit;
    *(int*)(sndmix + 192) = 3432;
    *(void (**)(void*, UNPACKINITPARAMS*))(sndmix + 68) = SFILTER_unpackmtpfinit;
    *(int*)(sndmix + 200) = 3444;
}
