// A fragment of stretch.c (0x80166dd8): stretch, which produces the time-
// stretch filter's output from its pending frames and then from the input
// through determineaction/applyaction, carrying a frame over when the last
// action stretched. stretchframesneeded before it is drafted in
// scratch/lib/stretch_wip.c (its final clamp compiles to a different idiom). Compiled as C++ like the rest of the
// sound library. The names come from the symbols; the state layout is inferred
// and its fields are named by offset.
struct TIMESTRETCHSTATE {
    unsigned char unknown00[28];
    unsigned char* m_1c;
    unsigned char unknown20[4];
    float m_24;
    float m_28;
    int m_framesize;
    int m_available;
    int m_carried;
    int m_pending;
    int m_position;
    float m_carry[510];
    float m_output[1];
};

int determineaction(int, float, unsigned char**, float*);
void applyaction(TIMESTRETCHSTATE*, float**, int);
extern "C" void SNDI_memcpy(void*, const void*, int);

int stretch(void* handle, int frames, float* in, float* out) {
    TIMESTRETCHSTATE* state = (TIMESTRETCHSTATE*)handle;
    int total;
    int count;
    int action;
    float* source;

    total = state->m_pending;
    source = &state->m_output[state->m_position];
    if (frames <= total)
        total = frames;
    SNDI_memcpy(out, source, total * sizeof(float));
    frames -= total;
    state->m_position += total;
    out += total;
    state->m_pending -= total;
    if (frames == 0)
        return total;
    action = 0;
    while (frames > 0) {
        if (state->m_available < state->m_framesize)
            break;
        if (state->m_available < state->m_framesize * 2)
            action = 0;
        else
            action = determineaction(state->m_framesize, state->m_24, &state->m_1c, &state->m_28);
        applyaction(state, &in, action);
        count = state->m_pending;
        source = &state->m_output[state->m_position];
        if (frames <= count)
            count = frames;
        SNDI_memcpy(out, source, count * sizeof(float));
        frames -= count;
        total += count;
        state->m_position += count;
        out += count;
        state->m_pending -= count;
    }
    if (action > 0) {
        SNDI_memcpy(state->m_carry, in, state->m_framesize * sizeof(float));
        state->m_carried = state->m_framesize;
        in += state->m_framesize;
    }
    if (frames > 0) {
        if (frames > state->m_available)
            frames = state->m_available;
        SNDI_memcpy(out, in, frames * sizeof(float));
        total += frames;
        state->m_available -= frames;
    }
    return total;
}
