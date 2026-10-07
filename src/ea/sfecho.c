/* The echo filter's state functions, the last of the file: restoring frees
   the delay buffer, creating installs the echo and restore functions and
   clears the state, and modifying sets the feedback gain from the first
   parameter (in 1/127 steps), sizes a 16-byte-aligned delay buffer from the
   other two and clears it (its float constants are entries of the file's
   .sdata2 pool). ECHOSTATE and the functions are named by the
   mangled symbols; the state layout is inferred. */
extern "C" void* memset(void*, int, unsigned long);

void SNDMEMI_free(void*);
void* SNDMEMI_allocz(int);
int SNDI_findmult16(int, int);

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

void SFILTER_modifyECHO(ECHOSTATE* state, int* params) {
    int feedback = params[0] >> 1;
    int delay = params[1] >> 8;
    int rate = params[2] >> 8;
    SFILTER_echorestore(state);
    state->gain = (1.0f / 127.0f) * feedback;
    state->negGain = -state->gain;
    state->length = SNDI_findmult16(delay, rate);
    {
        int size = state->length * 4;
        state->memory = SNDMEMI_allocz(size + 16);
        state->buffer = (int*)(((int)state->memory + 15) & ~15);
        memset(state->buffer, 0, size);
    }
    state->position = 0;
}
