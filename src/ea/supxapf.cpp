// Unpacker for EA-XA compressed audio fed by the packet player, decoding
// through SND::CEAXABLKDecf. UNPACKINITPARAMS and CEAXABLKDecf are named by the
// mangled symbols; the state record (UNPACKXAPACKETSTATE, not an original
// name) and the members are inferred from offsets.

namespace SND {
class CEAXABLKDecf {
public:
    CEAXABLKDecf();
    void* operator new(unsigned long);
    void operator delete(void*);

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
    int field30;
    unsigned char channel;
};

int SNDDRV_getmastervoice(int);
int SNDPKTPLAYI_voicetopackethandle(int);
int SNDDRV_getsamplechan(int);

// SFILTER_unpackxapf (the decode step) comes first in the original file and
// is not reconstructed, so this unit starts after it.
int SFILTER_unpackxapf(void*, int, void*, void*, int);

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
