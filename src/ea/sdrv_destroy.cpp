/* A fragment of the sound driver (snddrv.c, 0x8016bb50): destroying a
   packet player's platform record (freeing its two DSP buffers and
   unregistering it), the fixed output latency, and SNDPLATFORM_setvol, which
   applies a voice's volume (the product of its two volume bytes) to each of
   its channels: through the DSP mixer for hardware voices, else as dry gains
   of the software mixer per output. snddrv and sndgs are named by their
   symbols; their views, the voice and channel records and the platform
   record are inferred, and the scale factors are entries of the file's
   .sdata2 pool. */
void SNDPLATFORM_memfree(int, unsigned int);

/* inferred: the packet player's platform record */
struct SNDDRVPACKETVIEW {
    unsigned char buffer[4800];
    unsigned char* staging[3];
    signed char state;
    unsigned char unknown12cd[3];
    void* dsp[2];
    unsigned char unknown12d8[36];
};

struct _AXVPB;

/* inferred: a driver channel record (48 bytes): output levels, AX voice */
struct SNDDRVCHANNELVIEW {
    short levels[22];
    _AXVPB* voice;
};

struct SNDDRVVIEW {
    SNDDRVCHANNELVIEW channels[77];
    unsigned char unknown0e70[20];
    SNDDRVPACKETVIEW* packets[1];
};

/* inferred: a voice record (128 bytes), as in ssysreal.c */
struct SNDVOICEVIEW {
    unsigned char unknown00[4];
    short channels[13];
    unsigned char unknown1e;
    unsigned char channelCount;
    unsigned short flags;
    unsigned char unknown22[35];
    signed char volume;
    unsigned char unknown46[13];
    signed char gain;
    unsigned char unknown54[44];
};

struct SNDGSVIEW {
    unsigned char unknown000[51];
    unsigned char channelBase;
    unsigned char unknown034[20];
    unsigned char outputs;
    unsigned char unknown049[395];
    SNDVOICEVIEW* voices;
};

extern SNDGSVIEW sndgs;
void SNDDRV_DSPMixerSetVol(_AXVPB*, int);
extern "C" void SNDMIX_setdrygain(int, int, float);

extern SNDDRVVIEW snddrv;

int SNDPLATFORM_packetplaydestroy(int index) {
    SNDDRVPACKETVIEW* packet = snddrv.packets[index];

    SNDPLATFORM_memfree(0, (unsigned int)packet->dsp[0]);
    SNDPLATFORM_memfree(0, (unsigned int)packet->dsp[1]);
    snddrv.packets[index] = 0;
    return 0;
}

int SNDPLATFORM_outputlatency(void) {
    return -15;
}

void SNDPLATFORM_setvol(int index) {
    SNDVOICEVIEW* voice = &sndgs.voices[index];
    int i;
    int j;

    if (voice->flags & 0x200) {
        int volume = (int)(0.007874016f * ((float)voice->volume * (float)voice->gain));
        volume <<= 8;
        for (i = 0; i < voice->channelCount; i++)
            SNDDRV_DSPMixerSetVol(snddrv.channels[voice->channels[i]].voice, volume);
    } else {
        float gain = 1.8921513e-09f * ((float)voice->volume * (float)voice->gain);
        int outputs = sndgs.outputs == 1 ? 2 : sndgs.outputs;
        for (i = 0; i < voice->channelCount; i++) {
            SNDDRVCHANNELVIEW* channel = &snddrv.channels[voice->channels[i]];
            for (j = 0; j < outputs; j++)
                SNDMIX_setdrygain(voice->channels[i] - sndgs.channelBase, j, gain * (float)channel->levels[j]);
        }
    }
}
