// Looping unpacker for 16-bit PCM held in memory. UNPACKINITPARAMS is named by
// the mangled symbols; it and the state record (UNPACKLOOPSTATE, not an
// original name) have members inferred from offsets. sndmix's sample
// converter is read through an inferred offset.
extern "C" char sndmix[];

struct UNPACKINITPARAMS {
    short* data;
    int field4;
    int field8;
    unsigned int length;
    unsigned int loopstart;
    unsigned int loopend;
    int convert;
    int field1C;
    int field20;
    int (*getframe)(void*);
};

struct UNPACKLOOPSTATE {
    int (*unpack)(void*, int, void*, void*, int);
    char field4[24];
    short* data;
    unsigned int position;
    unsigned int loopstart;
    unsigned int loopend;
    int convert;
};

typedef void (*CONVERTFUNC)(int, short*, void*);

int SFILTER_unpacklfgetframe(void* state) {
    return ((UNPACKLOOPSTATE*)state)->position;
}

int SFILTER_unpacklf(void* data, int count, void* unused, void* output, int channels) {
    UNPACKLOOPSTATE* state = (UNPACKLOOPSTATE*)data;
    char* mix = sndmix;
    int n;
    float* out = (float*)output;

    while (count > 0) {
        n = state->loopend - state->position + 1;
        if (count < n)
            n = count;
        if (state->convert)
            (*(CONVERTFUNC*)(mix + 508))(n, state->data + state->position, out);
        state->position += n;
        out += n;
        count -= n;
        if (state->position > state->loopend)
            state->position = state->loopstart;
    }
    return 1;
}

void SFILTER_unpacklfinit(void* data, UNPACKINITPARAMS* params) {
    UNPACKLOOPSTATE* state = (UNPACKLOOPSTATE*)data;

    state->unpack = SFILTER_unpacklf;
    state->data = params->data;
    state->position = 0;
    state->loopstart = params->loopstart;
    state->loopend = params->loopend;
    state->convert = params->convert;
    params->getframe = SFILTER_unpacklfgetframe;
}
