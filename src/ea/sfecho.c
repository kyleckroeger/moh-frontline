/* The echo filter's restore and create functions: restoring frees the delay
   buffer, and creating installs the echo and restore functions and clears
   the state. SFILTER_modifyECHO after them (which inlines the restore) is not
   part of the unit: its float constants start at a 4-mod-8 .sdata2 address. ECHOSTATE and the functions are named by the
   mangled symbols; the state layout is inferred. */
void SNDMEMI_free(void*);

struct ECHOSTATE {
    void (*filter)(void*, int, void*, void*, int);
    void (*restore)(void*);
    int unknown08;
    int unknown0c;
    int unknown10;
    int unknown14;
    unsigned char unknown18[3];
    unsigned char unknown1b;
    int* buffer;
    void* memory;
    int position;
    int length;
    float gain;
    float negGain;
};

void SFILTER_echo(void*, int, void*, void*, int);

void SFILTER_echorestore(void* p) {
    ECHOSTATE* state = (ECHOSTATE*)p;
    if (state->memory) {
        SNDMEMI_free(state->memory);
        state->memory = 0;
        state->buffer = 0;
    }
}

int SFILTER_createECHO(ECHOSTATE* state) {
    state->unknown08 = 0;
    state->unknown0c = 0;
    state->unknown10 = 0;
    state->unknown14 = 0;
    state->unknown1b = 0;
    state->restore = SFILTER_echorestore;
    state->filter = SFILTER_echo;
    state->memory = 0;
    state->buffer = 0;
    return 0;
}
