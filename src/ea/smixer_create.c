// A fragment of the software mixer (smixer.c, 0x801602ec): MIX_create sets
// the coda allocation hooks, copies the creation options, allocates the two
// 64-byte aligned work buffers and the per-output mix buffers and the
// channel records (80 bytes each, cleared) inside the sound-system critical
// section, initialises the unpackers, resets the mixer, and gives every
// output a float-to-integer stage (priority 0) on its filter chain and a
// cleared mix buffer. sndmix and the functions are named by their symbols;
// the views (option and mixer layouts, and the filter-stage priority at +24
// as in sfilter.c) are inferred.
/* inferred: the creation options */
struct SNDMIXOPTSVIEW {
    unsigned int value00;
    unsigned char channels;
    unsigned char outputs;
    unsigned char value06;
    unsigned char value07;
    unsigned int value08;
};

/* inferred: a float-to-integer stage (FT24_32STATE) as a filter record */
struct FT24_32STATE {
    unsigned char unknown00[24];
    unsigned short priority;
    unsigned char unknown1a[2];
};

/* inferred: the software mixer's state, as far as it is used here */
struct SNDMIXVIEW {
    unsigned int value00;
    unsigned char channels;
    unsigned char outputs;
    unsigned char value06;
    unsigned char value07;
    unsigned int value08;
    unsigned char unknown00c[3];
    unsigned char value0f;
    unsigned char unknown010[272];
    void* work[2];
    int* alignedWork[2];
    void* mix[4];
    int* alignedMix[4];
    unsigned char unknown150[20];
    void* filters[4];
    unsigned char unknown174[16];
    FT24_32STATE ft[4];
    void* channelRecords;
};

namespace SND {
void CODASetNew(void* (*)(unsigned long));
void CODASetDelete(void (*)(void*));
}

void* SNDI_New(unsigned long);
void SNDI_Delete(void*);
void* SNDMEMI_allocz(int);
void SFILTER_ft24_32init(FT24_32STATE*);

extern "C" {
extern SNDMIXVIEW sndmix;
void SNDSYS_entercritical(void);
void SNDSYS_leavecritical(void);
void MIXI_initunpack16(void);
void MIXI_initunpackxa(void);
void MIXI_initunpackmt(void);
void MIX_reset(void);
void SFILTER_add(void*, void*);
void* memset(void*, int, unsigned long);
}

extern "C" void MIX_create(SNDMIXOPTSVIEW* opts) {
    int i;

    SND::CODASetNew(SNDI_New);
    SND::CODASetDelete(SNDI_Delete);
    sndmix.value0f = 0;
    sndmix.value00 = opts->value00;
    sndmix.channels = opts->channels;
    sndmix.outputs = opts->outputs;
    sndmix.value06 = opts->value06;
    sndmix.value07 = opts->value07;
    sndmix.value08 = opts->value08;
    SNDSYS_entercritical();
    for (i = 0; i < 2; i++) {
        sndmix.work[i] = SNDMEMI_allocz(5308);
        sndmix.alignedWork[i] = (int*)sndmix.work[i];
        sndmix.alignedWork[i] += 2;
        while ((unsigned long)sndmix.alignedWork[i] & 63)
            sndmix.alignedWork[i]++;
    }
    for (i = 0; i < sndmix.outputs; i++) {
        sndmix.mix[i] = SNDMEMI_allocz(704);
        sndmix.alignedMix[i] = (int*)sndmix.mix[i];
        while ((unsigned long)sndmix.alignedMix[i] & 63)
            sndmix.alignedMix[i]++;
    }
    if (sndmix.channels) {
        sndmix.channelRecords = SNDMEMI_allocz(sndmix.channels * 80);
        memset(sndmix.channelRecords, 0, sndmix.channels * 80);
    }
    SNDSYS_leavecritical();
    MIXI_initunpack16();
    MIXI_initunpackxa();
    MIXI_initunpackmt();
    MIX_reset();
    for (i = 0; i < sndmix.outputs; i++) {
        sndmix.filters[i] = 0;
        SFILTER_ft24_32init(&sndmix.ft[i]);
        sndmix.ft[i].priority = 0;
        SFILTER_add(&sndmix.filters[i], &sndmix.ft[i]);
        memset(sndmix.alignedMix[i], 0, 640);
    }
}
