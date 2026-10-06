// Looping unpacker for EA-XA compressed audio in memory, decoding through
// SND::CEAXABLKDecf. UNPACKINITPARAMS and CEAXABLKDecf are named by the mangled
// symbols; the state record (UNPACKXALOOPSTATE, not an original name) and the
// members are inferred from offsets.

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
    int loopstart;
    int loopend;
    int field18[3];
    int (*getframe)(void*);
};

struct UNPACKXALOOPSTATE {
    int (*unpack)(void*, int, void*, void*, int);
    void (*restore)(void*);
    char field8[20];
    SND::CEAXABLKDecf* decoder;
    void* data;
    int length;
    int position;
    int field2C;
    int field30;
    int loopstart;
    int loopend;
};

// SFILTER_unpackxalf (the looping decode step) comes first in the original
// file and is not reconstructed, so this unit starts after it.
int SFILTER_unpackxalf(void*, int, void*, void*, int);

int SFILTER_unpackgetframexalf(void* data) {
    return ((UNPACKXALOOPSTATE*)data)->position;
}

void SFILTER_unpackxalfrestore(void* data) {
    if (((UNPACKXALOOPSTATE*)data)->decoder)
        delete ((UNPACKXALOOPSTATE*)data)->decoder;
}

void SFILTER_unpackxalfinit(void* data, UNPACKINITPARAMS* params) {
    UNPACKXALOOPSTATE* state = (UNPACKXALOOPSTATE*)data;

    state->unpack = SFILTER_unpackxalf;
    state->restore = SFILTER_unpackxalfrestore;
    state->data = params->data;
    state->length = params->length;
    state->position = 0;
    state->field2C = 0;
    state->field30 = 0;
    state->loopstart = params->loopstart;
    state->loopend = params->loopend;
    state->decoder = new SND::CEAXABLKDecf;
    params->getframe = SFILTER_unpackgetframexalf;
}
