// Platform effect handling: select or restore the mixer reverb from bus 0's
// type, and apply a voice's effect send level either as a DSP aux bus level
// (hardware voices, flag 0x200) or as the software mixer's wet gain (flag 4).
// sndgs, sndmix and snddrv layouts are not known; their fields are accessed
// through inferred offsets. _AXVPB is named by the mangled symbols.
struct _AXVPB;

extern "C" {
extern char sndgs[];
extern char sndmix[];
extern struct SNDDRVVIEW snddrv;
extern char fxdefault[];
void SNDSYS_entercritical(void);
void SNDSYS_leavecritical(void);
void MIX_restorereverb(void);
void MIX_initreverb(int, void*);
void SNDMIX_setwetgain(int, int, float);
}
void SNDDRV_initmixer(void);
void SNDDRV_DSPMixerSetAuxBus(_AXVPB*, unsigned short, int);

void SNDDRV_setfx(int bus) {
    short type;

    SNDSYS_entercritical();
    type = *(short*)(sndgs + 476);
    if (type == 0) {
        *(int*)(sndmix + 16) = 0;
        *(int*)(sndmix + 20) = 0;
        MIX_restorereverb();
    } else {
        void* params;

        if (type == 1)
            params = *(void**)(sndgs + 480);
        else
            params = fxdefault;
        MIX_initreverb(*(unsigned short*)(sndgs + 42), params);
    }
    SNDSYS_leavecritical();
}

struct SNDDRVVIEW {
    char field0[3708];
    void (*setfx)(int);
    char fieldE80[40000];
    signed char mixerinited;
};

int SNDPLATFORM_fxinit(int group, int bus) {
    snddrv.setfx = SNDDRV_setfx;
    if (!snddrv.mixerinited)
        SNDDRV_initmixer();
    if (*(signed char*)(sndgs + 360))
        SNDDRV_setfx(bus);
    return 0;
}

struct SNDVOICEVIEW {
    char field0[4];
    short channel[13];
    char field1E;
    unsigned char channels;
    unsigned short flags;
    char field22[35];
    signed char volume;
    char field46[16];
    short fxlevel[2];
    char field5A[38];
};

int SNDPLATFORM_setfxlevel(int voice, int bus) {
    SNDVOICEVIEW* entry = &(*(SNDVOICEVIEW**)(sndgs + 468))[voice];
    int i;

    if (entry->flags & 0x200) {
        float level = (1.0f / 127.0f) * ((float)entry->volume * (float)entry->fxlevel[bus]);

        for (i = 0; i < entry->channels; i++)
            SNDDRV_DSPMixerSetAuxBus(*(_AXVPB**)((char*)&snddrv + entry->channel[i] * 48 + 44), level, 1);
    } else if (entry->flags & 4) {
        float gain = (1.0f / (127.0f * 32767.0f)) * ((float)entry->volume * (float)entry->fxlevel[bus]);

        for (i = 0; i < entry->channels; i++)
            SNDMIX_setwetgain(entry->channel[i] - *(unsigned char*)(sndgs + 51), bus, gain);
    } else {
        return -15;
    }
    return 0;
}
