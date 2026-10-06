// The low-pass FIR filter. Field meanings are not established beyond what
// the code shows: a filter starts with its process function and an optional
// input filter that is run first.
struct SNDFIRSTATE;

struct LPFSTATE {
    int (*process)(void*, int, void*, void*, int);
    int field04;
    LPFSTATE* input;
    int field0c;
    int field10;
    int field14;
    unsigned char unknown18[2];
    unsigned char field1a;
    unsigned char field1b;
    unsigned char fir[56];
    float cutoff;
};

void SNDI_fir8(SNDFIRSTATE*, int, void*, void*);
void SNDI_fir8init(SNDFIRSTATE*);
void calcFIRCoeffs(SNDFIRSTATE*, int);

// Unlike the high- and band-pass filters, this one filters the requested
// count after the input filter succeeds, not the count the input returned.
int SFILTER_lpfFIR8(void* filter, int count, void* source, void* destination, int) {
    LPFSTATE* state = static_cast<LPFSTATE*>(filter);
    if (state->input) {
        int result = state->input->process(state->input, count, destination, source, state->field1a);
        if (result <= 0)
            return result;
    }
    SNDI_fir8(reinterpret_cast<SNDFIRSTATE*>(state->fir), count, source, destination);
    return count;
}

int SFILTER_createLPFFIR8(LPFSTATE* state) {
    state->input = 0;
    state->field0c = 0;
    state->field10 = 0;
    state->field14 = 0;
    state->field1b = 0;
    state->field04 = 0;
    state->process = SFILTER_lpfFIR8;
    SNDI_fir8init(reinterpret_cast<SNDFIRSTATE*>(state->fir));
    return 0;
}

void SFILTER_modifyLPFFIR8(LPFSTATE* state, int* parameters) {
    state->cutoff = 2.0f * (float)(parameters[0] >> 8) / (float)(parameters[1] >> 8);
    calcFIRCoeffs(reinterpret_cast<SNDFIRSTATE*>(state->fir), 2);
}
