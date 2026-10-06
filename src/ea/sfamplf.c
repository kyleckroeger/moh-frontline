// The amplifier filter. Field meanings are not established beyond what the
// code shows: a filter starts with its process function and an optional input
// filter that is run first.
struct AMPLFSTATE {
    int (*process)(void*, int, void*, void*, int);
    int field04;
    AMPLFSTATE* input;
    int field0c;
    int field10;
    int field14;
    unsigned char unknown18[2];
    unsigned char field1a;
    unsigned char field1b;
    float gain;
};

int SFILTER_amplf(void* filter, int count, void* source, void* destination, int) {
    AMPLFSTATE* state = static_cast<AMPLFSTATE*>(filter);
    if (state->input) {
        int result = state->input->process(state->input, count, destination, source, state->field1a);
        if (result <= 0)
            return result;
    }
    float* in = static_cast<float*>(source);
    float* out = static_cast<float*>(destination);
    for (int i = 0; i < count; i++)
        out[i] = state->gain * in[i];
    return count;
}

int SFILTER_createAMPLF(AMPLFSTATE* state) {
    state->input = 0;
    state->field0c = 0;
    state->field10 = 0;
    state->field14 = 0;
    state->field1b = 0;
    state->field04 = 0;
    state->process = SFILTER_amplf;
    state->gain = 0.0f;
    return 0;
}

void SFILTER_modifyAMPLF(AMPLFSTATE* state, int* parameters) {
    state->gain = (1.0f / 256.0f) * parameters[0];
}
