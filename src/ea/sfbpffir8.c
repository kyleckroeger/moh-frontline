// The band-pass FIR filter. Field meanings are not established beyond what
// the code shows: a filter starts with its process function and an optional
// input filter that is run first.
struct SNDFIRSTATE;

struct BPFSTATE {
    int (*process)(void*, int, void*, void*, int);
    int field04;
    BPFSTATE* input;
    int field0c;
    int field10;
    int field14;
    unsigned char unknown18[2];
    unsigned char field1a;
    unsigned char field1b;
    unsigned char fir[52];
    float lowCutoff;
    float highCutoff;
};

void SNDI_fir8(SNDFIRSTATE*, int, void*, void*);
void SNDI_fir8init(SNDFIRSTATE*);
void calcFIRCoeffs(SNDFIRSTATE*, int);

int SFILTER_bpfFIR8(void* filter, int count, void* source, void* destination, int) {
    BPFSTATE* state = static_cast<BPFSTATE*>(filter);
    int samples;
    if (state->input) {
        samples = state->input->process(state->input, count, destination, source, state->field1a);
        if (samples <= 0)
            return samples;
    } else {
        samples = count;
    }
    SNDI_fir8(reinterpret_cast<SNDFIRSTATE*>(state->fir), samples, source, destination);
    return samples;
}

int SFILTER_createBPFFIR8(BPFSTATE* state) {
    state->input = 0;
    state->field0c = 0;
    state->field10 = 0;
    state->field14 = 0;
    state->field1b = 0;
    state->field04 = 0;
    state->process = SFILTER_bpfFIR8;
    SNDI_fir8init(reinterpret_cast<SNDFIRSTATE*>(state->fir));
    return 0;
}

void SFILTER_modifyBPFFIR8(BPFSTATE* state, int* parameters) {
    int low = parameters[0] >> 7;
    int high = parameters[1] >> 7;
    int rate = parameters[2] >> 8;
    state->lowCutoff = (float)low / (float)rate;
    state->highCutoff = (float)high / (float)rate;
    calcFIRCoeffs(reinterpret_cast<SNDFIRSTATE*>(state->fir), 4);
}
