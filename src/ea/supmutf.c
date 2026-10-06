// Unpacker for MicroTalk (UTALK) compressed audio: decode blocks of 432
// samples into a float buffer and copy them out, padding with silence after
// the end. UNPACKINITPARAMS and UTALKSTATE are named by the mangled symbols;
// the state record (UNPACKMTSTATE, not an original name) has members inferred
// from offsets.
struct UTALKSTATE {
    char data[1664];
};

struct UNPACKINITPARAMS {
    char* data;
    int field4;
    int field8;
    unsigned int length;
    int field10[5];
    int (*getframe)(void*);
};

struct UNPACKMTSTATE {
    int (*unpack)(void*, int, void*, void*, int);
    char field4[24];
    unsigned int length;
    unsigned int position;
    int available;
    UTALKSTATE utalk;
    float samples[432];
};

extern "C" void SNDI_memcpy(void*, const void*, int);
void decodemut(UTALKSTATE*);
void initmut(char*, UTALKSTATE*);

int SFILTER_unpackgetframemtf(void* data) {
    return ((UNPACKMTSTATE*)data)->position;
}

int SFILTER_unpackmtf(void* data, int count, void* unused, void* output, int channels) {
    UNPACKMTSTATE* state = (UNPACKMTSTATE*)data;
    float* out = (float*)output;
    int extra;
    int n;
    bool within;

    if (state->position >= state->length)
        return -1;
    state->position += count;
    extra = state->position - state->length;
    within = extra <= 0;
    if (within)
        extra = 0;
    if (!within)
        count -= extra;
    n = state->available;
    while (count > 0) {
        if (count < n)
            n = count;
        SNDI_memcpy(out, &state->samples[432 - state->available], n * 4);
        count -= n;
        out += n;
        state->available -= n;
        if (count > 0) {
            decodemut(&state->utalk);
            state->available = 432;
            n = 432;
        }
    }
    while (extra-- > 0)
        *out++ = 0.0f;
    return 1;
}

void SFILTER_unpackmtfinit(void* data, UNPACKINITPARAMS* params) {
    UNPACKMTSTATE* state = (UNPACKMTSTATE*)data;

    state->unpack = SFILTER_unpackmtf;
    state->length = params->length;
    state->position = 0;
    params->getframe = SFILTER_unpackgetframemtf;
    initmut(params->data, &state->utalk);
    state->available = 0;
}
