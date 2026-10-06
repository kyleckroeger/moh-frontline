// Fragment of sfbpffir8.c: the band-pass FIR filter's modify entry.
struct SNDFIRSTATE;

// Only the FIR state at +0x1c and the two cutoff ratios at +0x50/+0x54 are
// established.
struct BPFSTATE {
    unsigned char unknown00[28];
    unsigned char fir[52];
    float lowCutoff;
    float highCutoff;
};

void calcFIRCoeffs(SNDFIRSTATE*, int);

void SFILTER_modifyBPFFIR8(BPFSTATE* state, int* parameters) {
    int low = parameters[0] >> 7;
    int high = parameters[1] >> 7;
    int rate = parameters[2] >> 8;
    state->lowCutoff = (float)low / (float)rate;
    state->highCutoff = (float)high / (float)rate;
    calcFIRCoeffs(reinterpret_cast<SNDFIRSTATE*>(state->fir), 4);
}
