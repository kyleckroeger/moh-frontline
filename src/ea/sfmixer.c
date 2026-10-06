// The mixer filter: sums the output of two input filters. Field meanings
// beyond the code are not established.
struct MIXERSTATE {
    int (*process)(void*, int, void*, void*, int);
    void (*restore)(void*);
    MIXERSTATE* input;
    MIXERSTATE* input2;
    int field10;
    int field14;
    unsigned char unknown18[3];
    unsigned char field1b;
    float* buffer;
    int bufferCount;
};

void* SNDMEMI_allocz(int);
void SNDMEMI_free(void*);

int SFILTER_mixer(void* filter, int count, void* source, void* destination, int) {
    MIXERSTATE* state = static_cast<MIXERSTATE*>(filter);
    if (count > state->bufferCount) {
        if (state->buffer)
            SNDMEMI_free(state->buffer);
        state->bufferCount = count;
        state->buffer = static_cast<float*>(SNDMEMI_allocz(count * sizeof(float)));
    }
    int result = state->input->process(state->input, count, source, destination, 1);
    if (result <= 0)
        return result;
    result = state->input2->process(state->input2, count, source, state->buffer, 2);
    if (result <= 0)
        return result;
    float* out = static_cast<float*>(destination);
    for (int i = 0; i < count; i++)
        out[i] = out[i] + state->buffer[i];
    return count;
}

void SFILTER_mixerrestore(void* filter) {
    MIXERSTATE* state = static_cast<MIXERSTATE*>(filter);
    if (state->buffer)
        SNDMEMI_free(state->buffer);
}

int SFILTER_createMIX(MIXERSTATE* state) {
    state->input = 0;
    state->input2 = 0;
    state->field10 = 0;
    state->field14 = 0;
    state->field1b = 0;
    state->restore = SFILTER_mixerrestore;
    state->process = SFILTER_mixer;
    state->buffer = 0;
    state->bufferCount = 0;
    return 0;
}
