/* The resonance filter's process and create functions: processing runs an
   optional source plug-in first (stopping if it produces nothing) and then
   the x87-style resonator over the samples, and creating installs the
   process function and clears the state and filter history. RESONSTATE and
   the functions are named by the mangled symbols; the state layout and the
   plug-in view are inferred. resonx87 before these functions and
   SFILTER_modifyRESON after them are not part of the unit. */
struct SFILTERPLUGINVIEW {
    int (*process)(SFILTERPLUGINVIEW*, int, void*, void*, int);
};

struct RESONSTATE {
    int (*filter)(void*, int, void*, void*, int);
    void (*restore)(void*);
    SFILTERPLUGINVIEW* plugin;
    int unknown0c;
    int unknown10;
    int unknown14;
    unsigned char unknown18[2];
    unsigned char pluginFlags;
    unsigned char unknown1b;
    unsigned char unknown1c[24];
    float history[3];
};

void resonx87(RESONSTATE*, int, void*, void*);

int SFILTER_reson(void* p, int count, void* in, void* out, int) {
    RESONSTATE* state = (RESONSTATE*)p;
    int result;
    if (state->plugin) {
        result = state->plugin->process(state->plugin, count, out, in, state->pluginFlags);
        if (result <= 0)
            return result;
    } else {
        result = count;
    }
    resonx87(state, result, in, out);
    return result;
}

int SFILTER_createRESON(RESONSTATE* state) {
    state->plugin = 0;
    state->unknown0c = 0;
    state->unknown10 = 0;
    state->unknown14 = 0;
    state->unknown1b = 0;
    state->restore = 0;
    state->filter = SFILTER_reson;
    state->history[0] = 0.0f;
    state->history[1] = 0.0f;
    state->history[2] = 0.0f;
    return 0;
}
