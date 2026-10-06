/* SFILTER_rsfsetpitch, the first function of the resampling filter file: it
   stores the pitch in the filter state. RSFSTATE and the function are named
   by the mangled symbols; the state layout is inferred. */
struct RSFSTATE {
    unsigned char unknown00[28];
    int pitch;
};

void SFILTER_rsfsetpitch(RSFSTATE* state, int pitch) {
    state->pitch = pitch;
}
