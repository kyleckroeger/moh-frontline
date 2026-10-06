// MIX_setpitch: set a mixer channel's pitch (at most 4.0 in 16.16), creating
// its resampling stage on first use. sndmix and sndgs layouts are not known;
// their fields are accessed through inferred offsets. RSFSTATE is named by the
// mangled symbols.
extern "C" char sndmix[];
extern "C" char sndgs[];

struct RSFSTATE {
    int field0;
    int field4;
    char field8[16];
    short field18;
};

void* SNDMEMI_allocz(int);
void SFILTER_rsfinit(RSFSTATE*, int, int);
void SFILTER_rsfsetpitch(RSFSTATE*, int);
extern "C" void SFILTER_add(void*, void*);

extern "C" void MIX_setpitch(int channel, int pitch) {
    char* entry = *(char**)(sndmix + 500) + channel * 80;
    int highquality = 0;

    if (pitch > 0x40000)
        pitch = 0x40000;
    if (!*(RSFSTATE**)(entry + 64)) {
        *(RSFSTATE**)(entry + 64) = (RSFSTATE*)SNDMEMI_allocz(60);
        if (*(unsigned char*)(sndmix + 14) >= 50)
            highquality = 1;
        (*(RSFSTATE**)(entry + 64))->field4 = 0;
        (*(RSFSTATE**)(entry + 64))->field18 = 160;
        SFILTER_rsfinit(*(RSFSTATE**)(entry + 64), *(unsigned char*)(sndgs + 367), highquality);
        SFILTER_add(entry + 48, *(RSFSTATE**)(entry + 64));
    }
    SFILTER_rsfsetpitch(*(RSFSTATE**)(entry + 64), pitch);
}
