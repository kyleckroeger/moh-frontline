// Unpacker for EA-XA compressed audio fed by the packet player, decoding
// through SND::CEAXABLKDecf: SFILTER_unpackxapf decodes into the output,
// releasing the frames of the previous call and fetching the next packet
// (restarting the decoder from the packet's two-sample header) when the
// current one runs out, padding with silence when none is left. UNPACKINITPARAMS and CEAXABLKDecf are named by the
// mangled symbols; the state record (UNPACKXAPACKETSTATE, not an original
// name) and the members are inferred from offsets.

namespace SND {
/* inferred: the decoder's prediction state */
struct XAFSTATE {
    float previous;
    float current;
};

class CEAXABLKDecf {
public:
    CEAXABLKDecf();
    void* operator new(unsigned long);
    void operator delete(void*);
    int Decode(float**, int);
    void SetState(XAFSTATE*);
    void Feed(void*, int, int);

private:
    char m_state[168];
};
}

struct UNPACKINITPARAMS {
    void* data;
    int field4;
    int field8;
    int length;
    int field10[3];
    int voice;
};

struct UNPACKXAPACKETSTATE {
    int (*unpack)(void*, int, void*, void*, int);
    void (*restore)(void*);
    char field8[20];
    SND::CEAXABLKDecf* decoder;
    void* data;
    int length;
    int position;
    int packethandle;
    unsigned int field30;
    unsigned char channel;
};

int SNDDRV_getmastervoice(int);
void SNDPKTPLAYI_freeframes(int, int, int);
void* SNDPKTPLAYI_get(int, int, int*, int*);
extern "C" void* memset(void*, int, unsigned long);
int SNDPKTPLAYI_voicetopackethandle(int);
int SNDDRV_getsamplechan(int);

int SFILTER_unpackxapf(void* data, int count, void*, void* out, int) {
    UNPACKXAPACKETSTATE* state = (UNPACKXAPACKETSTATE*)data;
    int frames;
    int total = 0;
    float* destination = (float*)out;

    if (state->field30) {
        SNDPKTPLAYI_freeframes(state->packethandle, state->channel, state->field30);
        state->field30 = 0;
    }
    while (count > 0) {
        int decoded = state->decoder->Decode(&destination, count);
        state->position += decoded;
        total += decoded;
        destination += decoded;
        state->field30 += decoded;
        if (decoded < count) {
            count -= decoded;
            state->data = SNDPKTPLAYI_get(state->packethandle, state->channel, &state->length, &frames);
            if (state->data) {
                SND::XAFSTATE prediction;
                prediction.previous = ((short*)state->data)[0];
                prediction.current = ((short*)state->data)[1];
                state->decoder->SetState(&prediction);
                state->data = (char*)state->data + 4;
                state->position = 0;
                state->decoder->Feed(state->data, state->length * 4, state->length);
            } else {
                if (count > 0 && state->field30) {
                    memset(destination, 0, count * 4);
                    total += count;
                }
                count = 0;
            }
        } else {
            count -= decoded;
        }
    }
    return total;
}

void SFILTER_unpackxapfrestore(void* data) {
    if (((UNPACKXAPACKETSTATE*)data)->decoder)
        delete ((UNPACKXAPACKETSTATE*)data)->decoder;
}

void SFILTER_unpackxapfinit(void* data, UNPACKINITPARAMS* params) {
    UNPACKXAPACKETSTATE* state = (UNPACKXAPACKETSTATE*)data;

    state->unpack = SFILTER_unpackxapf;
    state->restore = SFILTER_unpackxapfrestore;
    state->packethandle = SNDPKTPLAYI_voicetopackethandle(SNDDRV_getmastervoice(params->voice));
    state->channel = SNDDRV_getsamplechan(params->voice);
    state->data = 0;
    state->length = params->length;
    state->position = 0;
    state->field30 = 0;
    state->decoder = new SND::CEAXABLKDecf;
}
