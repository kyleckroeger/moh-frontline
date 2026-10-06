// MIX_sethighpass: set a mixer channel's high-pass cutoff, creating its FIR
// stage on first use, or remove the stage for a cutoff of 0. sndmix and sndgs
// layouts are not known; their fields are accessed through inferred offsets.
// HPFSTATE is named by the mangled symbols.
extern "C" char sndmix[];
extern "C" char sndgs[];

struct HPFSTATE {
    int field0;
    int field4;
    char field8[16];
    short field18;
};

void* SNDMEMI_allocz(int);
void SNDMEMI_free(void*);
void SFILTER_createHPFFIR8(HPFSTATE*);
void SFILTER_modifyHPFFIR8(HPFSTATE*, int*);
extern "C" void SFILTER_add(void*, void*);
extern "C" void SFILTER_remove(void*, void*);

extern "C" void MIX_sethighpass(int channel, int frequency) {
    char* entry = *(char**)(sndmix + 500) + channel * 80;

    if (frequency > 0) {
        int params[2];

        if (!*(HPFSTATE**)(entry + 76)) {
            *(HPFSTATE**)(entry + 76) = (HPFSTATE*)SNDMEMI_allocz(88);
            (*(HPFSTATE**)(entry + 76))->field4 = 0;
            (*(HPFSTATE**)(entry + 76))->field18 = 80;
            SFILTER_createHPFFIR8(*(HPFSTATE**)(entry + 76));
            SFILTER_add(entry + 48, *(HPFSTATE**)(entry + 76));
        }
        params[0] = frequency << 8;
        params[1] = *(unsigned short*)(sndgs + 42) << 8;
        SFILTER_modifyHPFFIR8(*(HPFSTATE**)(entry + 76), params);
    } else if (*(HPFSTATE**)(entry + 76)) {
        SFILTER_remove(entry + 48, *(HPFSTATE**)(entry + 76));
        SNDMEMI_free(*(HPFSTATE**)(entry + 76));
        *(HPFSTATE**)(entry + 76) = 0;
    }
}
