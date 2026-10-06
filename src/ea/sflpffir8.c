// Fragment of sflpffir8.c: the low-pass FIR filter's modify entry.
struct SNDFIRSTATE;

// Only the FIR state at +0x1c and the cutoff ratio at +0x54 are established.
struct LPFSTATE {
    unsigned char unknown00[28];
    unsigned char fir[56];
    float cutoff;
};

void calcFIRCoeffs(SNDFIRSTATE*, int);

void SFILTER_modifyLPFFIR8(LPFSTATE* state, int* parameters) {
    state->cutoff = 2.0f * (float)(parameters[0] >> 8) / (float)(parameters[1] >> 8);
    calcFIRCoeffs(reinterpret_cast<SNDFIRSTATE*>(state->fir), 2);
}
