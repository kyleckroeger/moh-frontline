// The one-pole (RC) low-pass filter. Field meanings beyond the code are not
// established.
struct LPFRCSTATE {
    int (*process)(void*, int, void*, void*, int);
    int field04;
    LPFRCSTATE* input;
    int field0c;
    int field10;
    int field14;
    unsigned char unknown18[2];
    unsigned char field1a;
    unsigned char field1b;
    float output;
    float feedback;
    float gain;
};

int SFILTER_lpfRC(void* filter, int count, void* source, void* destination, int) {
    LPFRCSTATE* state = static_cast<LPFRCSTATE*>(filter);
    if (state->input) {
        int result = state->input->process(state->input, count, destination, source, state->field1a);
        if (result <= 0)
            return result;
    }
    float* in = static_cast<float*>(source);
    float* out = static_cast<float*>(destination);
    for (int i = 0; i < count; i++) {
        state->output = state->output * state->feedback + state->gain * in[i];
        out[i] = state->output;
    }
    return count;
}

int SFILTER_createLPFRC(LPFRCSTATE* state) {
    state->input = 0;
    state->field0c = 0;
    state->field10 = 0;
    state->field14 = 0;
    state->field1b = 0;
    state->field04 = 0;
    state->process = SFILTER_lpfRC;
    state->output = 0.0f;
    return 0;
}

void SFILTER_modifyLPFRC(LPFRCSTATE* state, int* parameters) {
    state->gain = 2.0f * parameters[0] / parameters[1];
    state->feedback = 1.0f - state->gain;
    state->gain = state->gain * ((1.0f / 256.0f) * parameters[2]);
    state->feedback = state->feedback * ((1.0f / 256.0f) * parameters[2]);
}
