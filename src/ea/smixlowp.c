// MIX_setlowpass: set a mixer channel's low-pass cutoff (a fraction of the
// sample rate), creating its RC filter stage on first use, or remove the stage
// for a cutoff of 1.0 or more. sndmix and sndgs layouts are not known; their
// fields are accessed through inferred offsets. LPFRCSTATE is named by the
// mangled symbols; this view of it is inferred.
extern "C" char sndmix[];
extern "C" char sndgs[];

struct LPFRCSTATE {
    int field0;
    int field4;
    char field8[16];
    short field18;
};

void* SNDMEMI_allocz(int);
void SNDMEMI_free(void*);
void SFILTER_createLPFRC(LPFRCSTATE*);
void SFILTER_modifyLPFRC(LPFRCSTATE*, int*);
extern "C" void SFILTER_add(void*, void*);
extern "C" void SFILTER_remove(void*, void*);

extern "C" void MIX_setlowpass(int channel, float cutoff) {
    char* entry = *(char**)(sndmix + 500) + channel * 80;

    if (cutoff < 1.0f) {
        int params[3];

        if (!*(LPFRCSTATE**)(entry + 72)) {
            *(LPFRCSTATE**)(entry + 72) = (LPFRCSTATE*)SNDMEMI_allocz(40);
            (*(LPFRCSTATE**)(entry + 72))->field4 = 0;
            (*(LPFRCSTATE**)(entry + 72))->field18 = 40;
            SFILTER_createLPFRC(*(LPFRCSTATE**)(entry + 72));
            SFILTER_add(entry + 48, *(LPFRCSTATE**)(entry + 72));
        }
        params[0] = (int)(cutoff * *(unsigned short*)(sndgs + 42)) << 7;
        params[1] = (unsigned short)*(unsigned short*)(sndgs + 42) << 8;
        params[2] = 256;
        SFILTER_modifyLPFRC(*(LPFRCSTATE**)(entry + 72), params);
    } else if (*(LPFRCSTATE**)(entry + 72)) {
        SFILTER_remove(entry + 48, *(LPFRCSTATE**)(entry + 72));
        SNDMEMI_free(*(LPFRCSTATE**)(entry + 72));
        *(LPFRCSTATE**)(entry + 72) = 0;
    }
}
