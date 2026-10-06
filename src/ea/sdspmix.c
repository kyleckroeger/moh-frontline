// The DSP mixer: per-voice mixer channels (64 records of 80 bytes) holding the
// pan, auxiliary bus levels and volume that are pushed to the AX voice
// parameter block when the mode flags say so. _AXVPB is named by the mangled
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
