// Unpacker for MicroTalk (UTALK) audio streamed through the packet player:
// fetch packets, decode blocks of 432 samples and copy them out, then return
// the consumed frames on the next call. UNPACKINITPARAMS and UTALKSTATE are
// named by the mangled symbols; the state record (UNPACKMTPSTATE, not an
// original name) and the decoder's first members are inferred from offsets.
struct UTALKSTATE {
    unsigned char* data;
    int field4;
    int field8;
    char fieldC[1652];
};

struct UNPACKINITPARAMS {
    char field0[28];
    int voice;
    int field20[2];
    int (*getframe)(void*);
};

struct UNPACKMTPSTATE {
    int (*unpack)(void*, int, void*, void*, int);
    char field4[24];
    unsigned char* packet;
    int packetlength;
    int handle;
    unsigned int consumed;
    unsigned short position;
    unsigned short available;
    UTALKSTATE utalk;
    float samples[432];
    unsigned char channel;
};

extern "C" {
void SNDI_memcpy(void*, const void*, int);
void* memset(void*, int, unsigned long);
}
void decodemut(UTALKSTATE*);
void initmut(char*, UTALKSTATE*);
void SNDPKTPLAYI_freeframes(int, int, int);
unsigned char* SNDPKTPLAYI_get(int, int, int*, int*);
int SNDDRV_getmastervoice(int);
int SNDDRV_getsamplechan(int);
int SNDPKTPLAYI_voicetopackethandle(int);

int SFILTER_unpackmtpf(void* data, int count, void* unused, void* output, int channels) {
    UNPACKMTPSTATE* state = (UNPACKMTPSTATE*)data;
    float* out = (float*)output;
    int n;

    if (state->consumed) {
        SNDPKTPLAYI_freeframes(state->handle, state->channel, state->consumed);
        state->consumed = 0;
    }
    while (count > 0) {
        if (state->position >= state->packetlength) {
            int flags;

            state->packet = SNDPKTPLAYI_get(state->handle, state->channel, &state->packetlength, &flags);
            state->position = 0;
            if (!state->packet) {
                if (state->consumed)
                    memset(out, 0, count * 4);
                state->packetlength = 0;
                return state->consumed;
            }
            if (state->packet[0]) {
                state->available = 0;
                initmut((char*)state->packet + 1, &state->utalk);
            } else {
                state->utalk.field4 = state->packet[1];
                state->utalk.field8 = 8;
                state->utalk.data = state->packet + 2;
            }
        }
        if (state->available == 0) {
            decodemut(&state->utalk);
            state->available = 432;
        }
        n = state->packetlength - state->position;
        if (count < n)
            n = count;
        if (state->available < n)
            n = state->available;
        SNDI_memcpy(out, &state->samples[432 - state->available], n * 4);
        out += n;
        count -= n;
        state->position += n;
        state->available -= n;
        state->consumed += n;
    }
    return state->consumed;
}

void SFILTER_unpackmtpfinit(void* data, UNPACKINITPARAMS* params) {
    UNPACKMTPSTATE* state = (UNPACKMTPSTATE*)data;

    state->unpack = SFILTER_unpackmtpf;
    state->handle = SNDPKTPLAYI_voicetopackethandle(SNDDRV_getmastervoice(params->voice));
    state->channel = SNDDRV_getsamplechan(params->voice);
    state->packet = 0;
    state->position = 0;
    state->packetlength = 0;
    state->available = 0;
    state->consumed = 0;
}
