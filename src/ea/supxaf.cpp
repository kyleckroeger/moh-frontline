// Unpacker for EA-XA compressed audio in memory, decoding through
// SND::CEAXABLKDecf (four-byte float samples; the tail of a short decode is
// zero-filled). UNPACKINITPARAMS and CEAXABLKDecf are named by the mangled
// symbols; the state record (UNPACKXASTATE, not an original name) and the
// members are inferred from offsets.

namespace SND {
class CEAXABLKDecf {
public:
    CEAXABLKDecf();
    void* operator new(unsigned long);
    void operator delete(void*);
    int Decode(float**, int);
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
    int field10[5];
    int (*getframe)(void*);
};

struct UNPACKXASTATE {
    int (*unpack)(void*, int, void*, void*, int);
    void (*restore)(void*);
    char field8[20];
    SND::CEAXABLKDecf* decoder;
    void* data;
    int length;
    int position;
};

// SFILTER_unpackxaf (the decode step) comes first in the original file; it
// is drafted in scratch but not matched, so this unit starts after it.
int SFILTER_unpackxaf(void*, int, void*, void*, int);

int SFILTER_unpackgetframexaf(void* data) {
    return ((UNPACKXASTATE*)data)->position;
}

void SFILTER_unpackxafrestore(void* data) {
    if (((UNPACKXASTATE*)data)->decoder)
        delete ((UNPACKXASTATE*)data)->decoder;
}

void SFILTER_unpackxafinit(void* data, UNPACKINITPARAMS* params) {
    UNPACKXASTATE* state = (UNPACKXASTATE*)data;

    state->unpack = SFILTER_unpackxaf;
    state->restore = SFILTER_unpackxafrestore;
    state->data = params->data;
    state->length = params->length;
    state->position = 0;
    state->decoder = new SND::CEAXABLKDecf;
    params->getframe = SFILTER_unpackgetframexaf;
    state->decoder->Feed(state->data, state->length * 4, state->length);
}
