// The high-pass FIR filter. Field meanings are not established beyond what
// the code shows: a filter starts with its process function and an optional
// input filter that is run first.
struct SNDFIRSTATE;

struct HPFSTATE {
    int (*process)(void*, int, void*, void*, int);
    int field04;
    HPFSTATE* input;
    int field0c;
    int field10;
    int field14;
    unsigned char unknown18[2];
    unsigned char field1a;
    unsigned char field1b;
    unsigned char fir[52];
    float cutoff;
};

void SNDI_fir8(SNDFIRSTATE*, int, void*, void*);
void SNDI_fir8init(SNDFIRSTATE*);
void calcFIRCoeffs(SNDFIRSTATE*, int);

int SFILTER_hpfFIR8(void* filter, int count, void* source, void* destination, int) {
    HPFSTATE* state = static_cast<HPFSTATE*>(filter);
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

int SFILTER_createHPFFIR8(HPFSTATE* state) {
    state->input = 0;
    state->field0c = 0;
    state->field10 = 0;
    state->field14 = 0;
    state->field1b = 0;
    state->field04 = 0;
    state->process = SFILTER_hpfFIR8;
    SNDI_fir8init(reinterpret_cast<SNDFIRSTATE*>(state->fir));
    return 0;
}

void SFILTER_modifyHPFFIR8(HPFSTATE* state, int* parameters) {
    state->cutoff = (float)(parameters[0] >> 7) / (float)(parameters[1] >> 8);
    calcFIRCoeffs(reinterpret_cast<SNDFIRSTATE*>(state->fir), 3);
}
