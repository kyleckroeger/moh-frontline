struct SNDFIRSTATE {
    float history[8];
};

void SNDI_fir8init(SNDFIRSTATE* state) {
    for (int i = 0; i < 8; i++)
        state->history[i] = 0.0f;
}
