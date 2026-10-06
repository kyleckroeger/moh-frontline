// The splitter filter: on alternate calls it pulls from its input and keeps a
// copy, then replays the copy. Field meanings beyond the code are not
// established.
struct SPLITSTATE {
    int (*process)(void*, int, void*, void*, int);
    void (*restore)(void*);
    SPLITSTATE* input;
    int field0c;
    int field10;
    int field14;
    unsigned char unknown18[3];
    unsigned char field1b;
    float* buffer;
    int bufferCount;
    short replayed;
    short pull;
};

void* SNDMEMI_allocz(int);
void SNDMEMI_free(void*);
extern "C" void SNDI_memcpy(void*, const void*, int);

int SFILTER_splitter(void* filter, int count, void* source, void* destination, int) {
    SPLITSTATE* state = static_cast<SPLITSTATE*>(filter);
    if (state->replayed) {
        state->replayed = 0;
        state->pull = 1;
    }
    int bytes = count * sizeof(float);
    if (state->bufferCount < count) {
        if (state->buffer)
            SNDMEMI_free(state->buffer);
        state->buffer = static_cast<float*>(SNDMEMI_allocz(bytes));
        state->bufferCount = count;
    }
    if (state->pull) {
        int result = state->input->process(state->input, count, destination, source, 1);
        if (result <= 0)
            return result;
        SNDI_memcpy(destination, source, bytes);
        SNDI_memcpy(state->buffer, source, bytes);
        state->pull = 0;
    } else {
        SNDI_memcpy(destination, state->buffer, bytes);
        state->replayed = 1;
    }
    return count;
}

void SFILTER_splitrestore(void* filter) {
    SPLITSTATE* state = static_cast<SPLITSTATE*>(filter);
    if (state->buffer)
        SNDMEMI_free(state->buffer);
}

int SFILTER_createSPLIT(SPLITSTATE* state) {
    state->input = 0;
    state->field0c = 0;
    state->field10 = 0;
    state->field14 = 0;
    state->field1b = 0;
    state->restore = SFILTER_splitrestore;
    state->process = SFILTER_splitter;
    state->bufferCount = 0;
    state->buffer = 0;
    state->replayed = 0;
    state->pull = 1;
    return 0;
}
