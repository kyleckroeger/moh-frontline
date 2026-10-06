// Fragment of sfamplf.c: the amplifier filter's create and modify entries.
// Field meanings are not established; offsets come from the stores.
struct AMPLFSTATE {
    void (*process)(void*, int, void*, void*, int);
    int field04;
    int field08;
    int field0c;
    int field10;
    int field14;
    unsigned char unknown18[3];
    unsigned char field1b;
    float gain;
};

void SFILTER_amplf(void*, int, void*, void*, int);

int SFILTER_createAMPLF(AMPLFSTATE* state) {
    state->field08 = 0;
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
