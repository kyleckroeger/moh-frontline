// The DSP mixer: per-voice mixer channels (64 records of 80 bytes) holding the
// pan, auxiliary bus levels and volume that are pushed to the AX voice
// parameter block when the mode flags say so. SNDDRV_DSPMixerInit resets every
// channel; SNDDRV_DSPMixerInitChannel binds a channel to its voice and writes
// its initial levels to the voice (EA's form of the SDK's MIXInit and
// MIXInitChannel). _AXVPB is named by the mangled
// symbols; the channel record's members and the AX voice offsets are
// inferred and are not original. This file is compiled with GC/1.3 (1.3.2
// builds the mode flag with oris instead of lis/or).
struct _AXVPB {
    char field000[24];
    unsigned long index;
    unsigned long sync;
    char field020[292];
    unsigned short srcSelect;
    unsigned short field146;
    unsigned short field148;
    unsigned short field14A;
    unsigned short field14C;
    unsigned short volume;
    unsigned short volumeDelta;
    unsigned short volumeL;
    unsigned short volumeLDelta;
    unsigned short volumeR;
    unsigned short volumeRDelta;
    char field156[6];
    unsigned short auxA;
    unsigned short field15E;
    unsigned short auxB;
    unsigned short field162;
    unsigned short auxC;
    unsigned short field16A;
    char field16C[48];
    unsigned short field19C;
    unsigned short field19E;
    unsigned short ve;
    unsigned short veDelta;
};

extern "C" {
int OSDisableInterrupts(void);
int OSRestoreInterrupts(int);
}

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

static MIXCHANNEL __MIXChannel[64];
static int numoutputchannels;

void SNDDRV_DSPMixerSetPan(_AXVPB* voice, unsigned short* pan) {
    MIXCHANNEL* channel = &__MIXChannel[voice->index];

    channel->levels[5] = pan[0];
    if (numoutputchannels != 3) {
        channel->levels[3] = pan[1];
    } else {
        channel->levels[7] = pan[1];
        channel->levels[3] = pan[2];
    }
    channel->mode |= 0x40000000;
}

void SNDDRV_DSPMixerSetAuxBus(_AXVPB* voice, unsigned short level, int bus) {
    MIXCHANNEL* channel = &__MIXChannel[voice->index];

    if (bus == 1) {
        channel->levels[11] = level;
        channel->levels[9] = level;
        if (numoutputchannels == 3)
            channel->levels[13] = level;
    } else if (bus == 2) {
        channel->levels[17] = level;
        channel->levels[15] = level;
        if (numoutputchannels == 3)
            channel->levels[19] = level;
    }
    channel->mode |= 0x40000000;
}

// SNDDRV_DSPMixerInit follows (drafted in scratch: the target unrolls its
// channel loop four times, this source twice), then the channel set-up,
// release, volume and update functions; they are not part of this unit.

void SNDDRV_DSPMixerInit(int outputs) {
    int i;

    for (i = 0; i < 64; i++) {
        __MIXChannel[i].mode = 0x50000000;
        __MIXChannel[i].volume = 0;
        __MIXChannel[i].field0C = 0;
        __MIXChannel[i].field10 = 0;
        __MIXChannel[i].levels[0] = __MIXChannel[i].levels[2] = __MIXChannel[i].levels[4] =
            __MIXChannel[i].levels[6] = __MIXChannel[i].levels[8] = __MIXChannel[i].levels[10] =
                __MIXChannel[i].levels[12] = __MIXChannel[i].levels[14] = __MIXChannel[i].levels[16] =
                    __MIXChannel[i].levels[18] = 0;
    }
    numoutputchannels = outputs;
}

void SNDDRV_DSPMixerInitChannel(_AXVPB* voice, unsigned long mode, int input, unsigned short auxA,
                                unsigned short auxB) {
    MIXCHANNEL* channel = &__MIXChannel[voice->index];
    int enabled;
    unsigned short mix;

    channel->voice = voice;
    channel->mode = mode & 4;
    channel->volume = input;
    channel->levels[9] = auxA;
    channel->levels[11] = auxA;
    channel->levels[15] = auxB;
    channel->levels[17] = auxB;
    if (numoutputchannels > 2) {
        channel->levels[13] = auxA;
        channel->levels[19] = auxB;
    }
    if (channel->mode & 4)
        channel->levels[0] = 0;
    else
        channel->levels[0] = input;
    enabled = OSDisableInterrupts();
    voice->field19C = channel->levels[0];
    voice->field19E = 0;
    voice->field14A = channel->levels[2];
    voice->field14C = 0;
    voice->volume = channel->levels[4];
    voice->volumeDelta = 0;
    voice->volumeL = channel->levels[6];
    voice->volumeLDelta = 0;
    mix = 0;
    if (voice->field162 || voice->field16A || voice->field15E)
        mix |= 4;
    voice->srcSelect = mix;
    voice->sync |= 0x212;
    OSRestoreInterrupts(enabled);
}
