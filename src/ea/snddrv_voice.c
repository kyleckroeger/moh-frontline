/* The sound driver's AX voice management and mixer callbacks (a fragment of
   snddrv.c): acquiring and releasing the AX voices behind a sound voice's
   channels, the DMA completion callback, the audio frame callback that feeds
   the software mixer's output to AX, and mixer creation. sndgs and snddrv are
   named by their symbols; the views of them are inferred from offsets.
   SNDDRV_callbackdropvoice, between this fragment and the one before it, is
   not reconstructed yet. */
struct _AXVPB;
struct OSThreadQueue;

extern "C" {
void AXFreeVoice(_AXVPB*);
_AXVPB* AXAcquireVoice(unsigned long, void (*)(void*), unsigned long);
extern char sndgs[];
int OSEnableInterrupts(void);
int OSRestoreInterrupts(int);
void OSWakeupThread(OSThreadQueue*);
float* MIX_getwetbuffer(void);
void SNDI_memcpy(void*, const void*, int);
void SNDSYS_linkmaincpumixer(void);
void SNDSYS_entercritical(void);
void SNDSYS_leavecritical(void);
extern void* MIXinitfn;
}
void SNDVOICEI_free(int);
void* SNDMEMI_allocz(int);
void SNDDRV_mixvoicefree(int);
void SNDDRV_callbackdropvoice(void*);
void SNDDRV_DSPMixerInitChannel(_AXVPB*, unsigned long, int, unsigned short, unsigned short);
void SNDDRV_DSPMixerReleaseChannel(_AXVPB*);

/* inferred view of snddrv: 61 per-channel records, then the DMA request
   table, the mixer flags and the mix buffers */
struct SNDDRVVOICE {
    char field00[8];
    int field08;
    char field0C[3];
    unsigned char active;
    char field10[4];
    unsigned char field14;
    unsigned char field15;
    char field16[22];
    _AXVPB* axvoice;
};

struct SNDDRVDMA {
    int id;
    char field04[60];
};

struct OSThreadQueue {
    void* head;
    void* tail;
};

struct SNDDRVVIEW {
    SNDDRVVOICE voices[61];
    char fieldb70[0x1858 - 61 * 48];
    OSThreadQueue threadqueue;
    char field1860[0x9860 - 0x1860];
    int lastdma;
    char field9864[0x98a8 - 0x9864];
    SNDDRVDMA dma[64];
    char fielda8a8[0xaac0 - 0xa8a8];
    signed char mixerinited;
    unsigned char activevoices;
    char fieldaac2[0xaac8 - 0xaac2];
    void* mixalloc[4];
    void* mixbuffer[3];
    char fieldaae4[0xab20 - 0xaae4];
};

extern SNDDRVVIEW snddrv;

/* sndgs's voice table entries (128 bytes) */
struct SNDVOICEVIEW {
    char field0[4];
    short channel[13];
    char field1E;
    unsigned char channels;
    char field20[4];
    short master;
    unsigned char priority;
    char field27[54];
    signed char field5D;
    char field5E[0x22];
};

int SNDDRV_allocateaxvoice(int voice) {
    int i;
    SNDVOICEVIEW* entry = &(*(SNDVOICEVIEW**)(sndgs + 468))[voice];

    for (i = 0; i < entry->channels; i++) {
        int priority;

        if (entry->priority == 101)
            priority = 31;
        else
            priority = entry->priority / 3;
        if (priority <= 0)
            priority = 1;
        else if (priority > 31)
            priority = 30;
        if ((snddrv.voices[entry->channel[i]].axvoice = AXAcquireVoice(priority, SNDDRV_callbackdropvoice, entry->channel[i])) != 0) {
            snddrv.voices[entry->channel[i]].field15 = 0;
            SNDDRV_DSPMixerInitChannel(snddrv.voices[entry->channel[i]].axvoice, 0, 0, 0, 0);
        } else {
            snddrv.voices[entry->channel[i]].field15 = 1;
            if (i > 0) {
                for (; i >= 0; i--) {
                    if (snddrv.voices[entry->channel[i]].axvoice) {
                        AXFreeVoice(snddrv.voices[entry->channel[i]].axvoice);
                        snddrv.voices[entry->channel[i]].field15 = 1;
                        snddrv.voices[entry->channel[i]].axvoice = 0;
                    }
                    if ((*(SNDVOICEVIEW**)(sndgs + 468))[entry->channel[i]].field5D)
                        SNDVOICEI_free(entry->channel[i]);
                }
            }
            return 0;
        }
    }
    return 1;
}

void SNDDRV_freeaxvoice(int channel, int mode) {
    SNDVOICEVIEW* entry = &(*(SNDVOICEVIEW**)(sndgs + 468))[channel];
    SNDDRVVOICE* drv = &snddrv.voices[channel];
    _AXVPB* axvoice = drv->axvoice;

    SNDDRV_DSPMixerReleaseChannel(axvoice);
    drv->field08 = -1;
    if (entry->field5D)
        SNDVOICEI_free(channel);
    if (mode == 0) {
        if (axvoice) {
            AXFreeVoice(axvoice);
            drv->axvoice = 0;
        }
    } else if (mode == 2) {
        if (!drv->field15 && axvoice) {
            drv->active = 1;
            snddrv.activevoices++;
        }
    } else if (mode == 1) {
        if (drv->active == 1) {
            drv->active = 0;
            snddrv.activevoices--;
        }
        drv->axvoice = 0;
    }
    if (!drv->field15)
        drv->field15 = 1;
}

struct SNDDMAREQVIEW {
    char field0[4];
    int id;
};

void SNDDRV_dmcallback(unsigned long request) {
    int i;

    snddrv.lastdma = ((SNDDMAREQVIEW*)request)->id;
    for (i = 0; i < 64; i++) {
        if (snddrv.dma[i].id == ((SNDDMAREQVIEW*)request)->id) {
            snddrv.dma[i].id = -1;
            return;
        }
    }
}

static int everyother;

void SNDDRV_audiocallback(void* buffers, void* context) {
    int enabled = OSEnableInterrupts();

    if (snddrv.mixerinited) {
        static float* ptr;
        float* dst;
        int* src;
        int i;

        dst = ptr = MIX_getwetbuffer();
        src = ((int**)buffers)[0];
        for (i = 0; i < 160; i++)
            *dst++ = (float)*src++;
        SNDI_memcpy(((void**)buffers)[1], snddrv.mixbuffer[0], 640);
        if (*(unsigned char*)(sndgs + 72) < 3) {
            SNDI_memcpy(((void**)buffers)[0], snddrv.mixbuffer[1], 640);
        } else {
            SNDI_memcpy(((void**)buffers)[0], snddrv.mixbuffer[2], 640);
            SNDI_memcpy(((void**)buffers)[2], snddrv.mixbuffer[1], 640);
        }
    }
    OSWakeupThread(&snddrv.threadqueue);
    everyother++;
    OSRestoreInterrupts(enabled);
}

/* inferred: the mixer's creation parameters */
struct MIXINITVIEW {
    int rate;
    unsigned char field04;
    unsigned char channels;
    unsigned char field06;
    void (*voicefree)(int);
};

void SNDDRV_initmixer() {
    MIXINITVIEW params;
    int i;

    SNDSYS_linkmaincpumixer();
    params.field04 = *(unsigned char*)(sndgs + 46);
    if (*(unsigned char*)(sndgs + 72) != 1)
        params.channels = *(unsigned char*)(sndgs + 72);
    else
        params.channels = 2;
    params.rate = *(unsigned short*)(sndgs + 42);
    params.voicefree = SNDDRV_mixvoicefree;
    params.field06 = 0;
    ((void (*)(MIXINITVIEW*))MIXinitfn)(&params);
    for (i = 0; i < params.channels; i++) {
        SNDSYS_entercritical();
        snddrv.mixalloc[i] = SNDMEMI_allocz(672);
        snddrv.mixbuffer[i] = (void*)(((unsigned long)snddrv.mixalloc[i] + 31) & ~31);
        SNDSYS_leavecritical();
    }
    snddrv.mixerinited = 1;
}
