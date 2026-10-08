// A fragment of the software mixer (smixer.c, 0x801605bc): MIX_playinit
// prepares a channel record (80 bytes): it clears its filter chain and
// stages, picks the unpacker for the sample format and mode from the
// mixer's table (allocating its state with priority 240 and initialising it
// from the play parameters), adds a time-stretch stage (priority 200) when
// one is given, and marks the record ready; MIX_play starts it (gain 0, the
// current volumes from the targets). sndmix, sndgs and the functions are
// named by their symbols; the record, parameter and mixer views and the
// meaning of the arguments are inferred.
/* inferred: a filter stage's header (as in sfilter.c) */
struct SNDSTAGEVIEW {
    unsigned char unknown00[4];
    int value04;
    unsigned char unknown08[16];
    unsigned short priority;
    unsigned char value1a;
};

/* inferred: the parameters handed to an unpacker's init function */
struct SNDUNPACKPARAMSVIEW {
    int value00;
    int value04;
    int value08;
    int value0c;
    int value10;
    int value14;
    int value18;
    int channel;
    int value20;
    int result;
};

/* inferred: a mixer channel record */
struct SNDMIXCHANNELVIEW {
    unsigned char state;
    unsigned char value01;
    unsigned char unknown02[2];
    float volume[4];
    float target[4];
    float value24;
    float value28;
    float gain;
    void* filters;
    int value34;
    SNDSTAGEVIEW* unpack;
    SNDSTAGEVIEW* stretch;
    int value40;
    unsigned char unknown44[4];
    int value48;
    int value4c;
};

typedef void (*SNDUNPACKINIT)(SNDSTAGEVIEW*, SNDUNPACKPARAMSVIEW*);

/* inferred: the software mixer's state, as far as it is used here */
struct SNDMIXVIEW {
    unsigned char unknown000[5];
    unsigned char outputs;
    unsigned char unknown006[18];
    SNDUNPACKINIT unpackInit[33];
    int unpackSize[86];
    SNDMIXCHANNELVIEW* channels;
};

struct SNDGSVIEW {
    unsigned char unknown000[367];
    unsigned char value16f;
};

struct TIMESTRETCHSTATE;

void* SNDMEMI_allocz(int);
void SFILTER_timestretchinit(TIMESTRETCHSTATE*, unsigned char*, int);

extern "C" {
extern SNDMIXVIEW sndmix;
extern SNDGSVIEW sndgs;
void SFILTER_add(void*, void*);
}

extern "C" void MIX_playinit(int index, int format, int mode, int value3, int value4, unsigned char* stretch,
                             int value6, int value7, int value8, int value9, int, int, int value12) {
    SNDMIXCHANNELVIEW* channel = &sndmix.channels[index];
    SNDUNPACKPARAMSVIEW params;
    int unpack;

    channel->filters = 0;
    channel->unpack = 0;
    channel->value40 = 0;
    channel->value48 = 0;
    channel->value4c = 0;
    channel->stretch = 0;
    params.value18 = 1;
    params.result = 0;
    unpack = 0;
    if (format == 8)
        unpack = 3;
    else if (format == 7)
        unpack = 3;
    else if (format == 9) {
        params.value18 = 0;
        unpack = 0;
    } else if (format == 10)
        unpack = 6;
    else if (format == 4)
        unpack = 9;
    else if (format == 14)
        unpack = 12;
    else if (format == 15)
        unpack = 15;
    else if (format == 16)
        unpack = 18;
    else if (format == 64)
        unpack = 21;
    if (mode == 1) {
        if (value9 > 0)
            unpack += 1;
    } else if (mode == 0) {
        unpack += 2;
    }
    if (sndmix.unpackInit[unpack]) {
        channel->unpack = (SNDSTAGEVIEW*)SNDMEMI_allocz(sndmix.unpackSize[unpack]);
        params.value00 = value3;
        params.value04 = value4;
        params.value08 = value6;
        params.value0c = value7;
        params.value10 = value8;
        params.value14 = value9;
        params.channel = index;
        params.value20 = sndgs.value16f;
        channel->unpack->value04 = 0;
        channel->unpack->priority = 240;
        channel->unpack->value1a = value12;
        sndmix.unpackInit[unpack](channel->unpack, &params);
        channel->value34 = params.result;
        SFILTER_add(&channel->filters, channel->unpack);
    }
    if (stretch) {
        if (mode)
            index = -1;
        channel->stretch = (SNDSTAGEVIEW*)SNDMEMI_allocz(6184);
        channel->stretch->value04 = 0;
        channel->stretch->priority = 200;
        SFILTER_timestretchinit((TIMESTRETCHSTATE*)channel->stretch, stretch, index);
        SFILTER_add(&channel->filters, channel->stretch);
    }
    channel->state = 1;
}

extern "C" void MIX_play(int index) {
    SNDMIXCHANNELVIEW* channel = &sndmix.channels[index];
    int i;

    channel->gain = 0.0f;
    channel->value01 = 0;
    channel->value24 = channel->value28;
    for (i = 0; i < sndmix.outputs; i++)
        channel->volume[i] = channel->target[i];
    channel->state = 2;
}
