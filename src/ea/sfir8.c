// Eight-tap symmetric FIR filter shared by the sound filters.
struct SNDFIRSTATE {
    float history[8];
    float coeffs[5];
};

void SNDI_fir8init(SNDFIRSTATE* state) {
    for (int i = 0; i < 8; i++)
        state->history[i] = 0.0f;
}

void SNDI_fir8(SNDFIRSTATE* state, int count, void* source, void* destination) {
    float* in = static_cast<float*>(source);
    float* out = static_cast<float*>(destination);
    for (int n = 0; n < count; n++) {
        *out++ = state->coeffs[0] * (state->history[7] + *in) + state->coeffs[1] * (state->history[0] + state->history[6]) +
                 state->coeffs[2] * (state->history[1] + state->history[5]) +
                 state->coeffs[3] * (state->history[2] + state->history[4]) + state->coeffs[4] * state->history[3];
        for (int i = 7; i > 0; i--)
            state->history[i] = state->history[i - 1];
        state->history[0] = *in++;
    }
}
