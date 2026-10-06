// MIXI_reverbblock: one comb filter stage of the mixer reverb, with a
// one-pole low-pass in the feedback path. The 1e-30 offset keeps the filter
// out of denormals. The state record's members are inferred from offsets and
// are not original.
struct REVERBSTATE {
    int writepos;
    int readpos;
    int field8;
    float* buffer;
    float feedback;
    float feedforward;
    float lowpass;
    float lowpasscoef;
    float inputcoef;
};

extern "C" void MIXI_reverbblock(REVERBSTATE* state, int count, float* source, float* dest) {
    float* write = state->buffer + state->writepos;
    float* read = state->buffer + state->readpos;

    for (; count > 0; count--) {
        float in = 1e-30f + *source++;
        float delayed = *--read + state->feedforward * in;

        float feedback = delayed * state->feedback;

        write[-1] = feedback + in;
        *dest++ = delayed;
        state->lowpass = state->inputcoef * write[-1] + state->lowpass * state->lowpasscoef;
        *--write = state->lowpass;
    }
}
