// Unpacker for 16-bit PCM held in memory. UNPACKINITPARAMS is named by the
// mangled symbols; it and the state record (UNPACKSTATE, not an original
// name) have members inferred from offsets. sndmix's sample converter is read
// through an inferred offset.
extern "C" {
extern char sndmix[];
void* memset(void*, int, unsigned long);
}

struct UNPACKINITPARAMS {
    short* data;
    int field4;
    int field8;
    unsigned int length;
    int field10;
    int field14;
    int convert;
    int field1C;
    int field20;
    int (*getframe)(void*);
};

struct UNPACKSTATE {
    int (*unpack)(void*, int, void*, void*, int);
    char field4[24];
    short* data;
    unsigned int position;
    unsigned int length;
    int convert;
};

typedef void (*CONVERTFUNC)(int, short*, void*);

int SFILTER_unpackfgetframe(void* state) {
    return ((UNPACKSTATE*)state)->position;
}

int SFILTER_unpackf(void* data, int count, void* unused, void* output, int channels) {
    UNPACKSTATE* state = (UNPACKSTATE*)data;
    unsigned int position = state->position;

    if (position >= state->length)
        return -1;
    state->position = position + count;
    if (state->position < state->length) {
        if (state->convert)
            (*(CONVERTFUNC*)(sndmix + 508))(count, state->data + position, output);
    } else {
        int remaining = state->length - position;

        if (state->convert)
            (*(CONVERTFUNC*)(sndmix + 508))(remaining, state->data + position, output);
        memset((int*)output + remaining, 0, (count - remaining) * 4);
    }
    return 1;
}

void SFILTER_unpackfinit(void* data, UNPACKINITPARAMS* params) {
    UNPACKSTATE* state = (UNPACKSTATE*)data;

    state->unpack = SFILTER_unpackf;
    state->data = params->data;
    state->position = 0;
    state->length = params->length;
    state->convert = params->convert;
    params->getframe = SFILTER_unpackfgetframe;
}
