// The source filter: copies samples from a caller-supplied buffer.
// Field meanings are not established beyond what the code shows.
struct SRCSTATE {
    int (*process)(void*, int, void*, void*, int);
    int field04;
    void* input;
    int field0c;
    int field10;
    int field14;
    unsigned char unknown18[3];
    unsigned char field1b;
    float* samples;
};

extern "C" void SNDI_memcpy(void*, const void*, int);

int SFILTER_src(void* filter, int count, void*, void* destination, int) {
    SRCSTATE* state = static_cast<SRCSTATE*>(filter);
    SNDI_memcpy(destination, state->samples, count * sizeof(float));
    state->samples += count;
    return count;
}

void SFILTER_initSOURCE(SRCSTATE* state, void* samples) {
    state->process = SFILTER_src;
    state->field04 = 0;
    state->samples = static_cast<float*>(samples);
}

int SFILTER_createSOURCE(SRCSTATE* state) {
    state->input = 0;
    state->field0c = 0;
    state->field10 = 0;
    state->field14 = 0;
    state->field1b = 0;
    state->samples = 0;
    return 0;
}
