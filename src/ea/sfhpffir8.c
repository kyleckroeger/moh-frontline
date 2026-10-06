// Fragment of sfhpffir8.c: the high-pass FIR filter's modify entry.
struct SNDFIRSTATE;

// Only the FIR state at +0x1c and the cutoff ratio at +0x50 are established.
struct HPFSTATE {
    unsigned char unknown00[28];
    unsigned char fir[52];
    float cutoff;
};

void calcFIRCoeffs(SNDFIRSTATE*, int);

void SFILTER_modifyHPFFIR8(HPFSTATE* state, int* parameters) {
    state->cutoff = (float)(parameters[0] >> 7) / (float)(parameters[1] >> 8);
    calcFIRCoeffs(reinterpret_cast<SNDFIRSTATE*>(state->fir), 3);
}
