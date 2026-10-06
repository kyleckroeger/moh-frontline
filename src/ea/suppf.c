// Unpacker for 16-bit PCM streamed through the packet player. UNPACKINITPARAMS
// is named by the mangled symbols; the state record (UNPACKPSTATE, not an
// original name) has members inferred from offsets. sndmix's sample converter
// is read through an inferred offset.
struct UNPACKINITPARAMS {
    char field0[24];
    int convert;
    int voice;
    int field20[2];
    int (*getframe)(void*);
};

struct UNPACKPSTATE {
    int (*unpack)(void*, int, void*, void*, int);
    char field4[24];
    short* packet;
    int packetlength;
    int position;
    int handle;
    unsigned int consumed;
    unsigned char convert;
    unsigned char channel;
};

typedef void (*CONVERTFUNC)(int, short*, void*);

struct SNDMIXVIEW {
    char field0[508];
    CONVERTFUNC convert;
};

extern "C" {
extern SNDMIXVIEW sndmix;
void* memset(void*, int, unsigned long);
}
void SNDPKTPLAYI_freeframes(int, int, int);
short* SNDPKTPLAYI_get(int, int, int*, int*);
int SNDDRV_getmastervoice(int);
int SNDDRV_getsamplechan(int);
int SNDPKTPLAYI_voicetopackethandle(int);

int SFILTER_unpackpf(void* data, int count, void* unused, void* output, int channels) {
    UNPACKPSTATE* state = (UNPACKPSTATE*)data;
    float* out = (float*)output;
    int n;
    SNDMIXVIEW* mix;

    if (state->consumed) {
        SNDPKTPLAYI_freeframes(state->handle, state->channel, state->consumed);
        state->consumed = 0;
    }
    mix = &sndmix;
    while (count > 0) {
        if (state->position >= state->packetlength) {
            int length;
            int flags;

            state->packet = SNDPKTPLAYI_get(state->handle, state->channel, &length, &flags);
            if (!state->packet) {
                if (state->consumed)
                    memset(out, 0, count * 4);
                return state->consumed;
            }
            state->position = 0;
            state->packetlength = length;
        }
        n = state->packetlength - state->position;
        if (count < n)
            n = count;
        if (state->convert)
            mix->convert(n, state->packet + state->position, out);
        state->position += n;
        out += n;
        count -= n;
        state->consumed += n;
    }
    return state->consumed;
}

void SFILTER_unpackpfinit(void* data, UNPACKINITPARAMS* params) {
    UNPACKPSTATE* state = (UNPACKPSTATE*)data;

    state->unpack = SFILTER_unpackpf;
    state->handle = SNDPKTPLAYI_voicetopackethandle(SNDDRV_getmastervoice(params->voice));
    state->channel = SNDDRV_getsamplechan(params->voice);
    state->packet = 0;
    state->packetlength = -1;
    state->position = 0;
    state->convert = params->convert;
    state->consumed = 0;
}
