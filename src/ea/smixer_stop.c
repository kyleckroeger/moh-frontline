// A fragment of the software mixer (smixer.c, 0x8016085c): MIX_stop adds a
// channel's remaining contribution to the mixer's level totals, runs and
// frees its pending filter stages, and marks the channel idle. sndmix is
// named by its symbol; its view and the channel and filter records are
// inferred.
/* inferred: a filter stage chained on a mixer channel */
struct MIXFILTERVIEW {
    unsigned char unknown00[4];
    void (*release)(MIXFILTERVIEW*);
    MIXFILTERVIEW* next;
};

/* inferred: a mixer channel's record (80 bytes) */
struct MIXCHANNELVIEW {
    unsigned char active;
    unsigned char unknown01[3];
    float outputs[4];
    unsigned char unknown14[16];
    float level;
    unsigned char unknown28[4];
    float gain;
    MIXFILTERVIEW* filters;
    unsigned char unknown34[28];
};

/* inferred: the software mixer's state, as far as it is used here */
struct SNDMIXVIEW {
    unsigned char unknown000[5];
    unsigned char outputs;
    unsigned char unknown006[330];
    float totals[4];
    float total;
    unsigned char unknown164[144];
    MIXCHANNELVIEW* channels;
};

extern "C" {
extern SNDMIXVIEW sndmix;
}
void SNDMEMI_free(void*);

extern "C" void MIX_stop(int index) {
    MIXCHANNELVIEW* channel = &sndmix.channels[index];
    MIXFILTERVIEW* next;
    int i;

    sndmix.total += channel->level * channel->gain;
    for (i = 0; i < sndmix.outputs; i++)
        sndmix.totals[i] += channel->outputs[i] * channel->gain;
    do {
        if (channel->filters->release)
            channel->filters->release(channel->filters);
        next = channel->filters->next;
        SNDMEMI_free(channel->filters);
        channel->filters = next;
    } while (next);
    channel->active = 0;
}
