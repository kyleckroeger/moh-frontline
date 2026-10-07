// A fragment of the DSP mixer (sdspmix.c, 0x8016d2e8): releasing a voice's
// mixer channel (clearing its voice) and setting its volume (flagging the
// volume for the next update). _AXVPB is named by the mangled symbols; the
// channel record's members and the AX voice offsets are inferred and are not
// original (see sdspmix.c). __MIXChannel is file-static in the original.
// Compiled with GC/1.3 like sdspmix.c.
struct _AXVPB {
    char field000[24];
    unsigned long index;
    unsigned long sync;
    char field020[292];
    unsigned short srcSelect;
    unsigned short field146;
    char field148[6];
    unsigned short volume;
    unsigned short volumeDelta;
    unsigned short volumeL;
    unsigned short volumeLDelta;
    unsigned short volumeR;
    unsigned short volumeRDelta;
    char field156[6];
    unsigned short auxA;
    char field15E[2];
    unsigned short auxB;
    char field162[2];
    unsigned short auxC;
    char field166[54];
    unsigned short ve;
    unsigned short veDelta;
};

typedef struct MIXCHANNEL {
    _AXVPB* voice;
    unsigned long mode;
    int volume;
    int field0C;
    int field10;
    char field14[20];
    // Levels at +0x28..+0x4E; their meanings are not established.
    unsigned short levels[20];
} MIXCHANNEL;

// File-local in the original (defined in sdspmix.c's part of the file).
extern MIXCHANNEL __MIXChannel[64];

void SNDDRV_DSPMixerReleaseChannel(_AXVPB* voice) {
    __MIXChannel[voice->index].voice = 0;
}

void SNDDRV_DSPMixerSetVol(_AXVPB* voice, int volume) {
    MIXCHANNEL* channel = &__MIXChannel[voice->index];

    channel->volume = volume;
    channel->mode |= 0x10000000;
}
