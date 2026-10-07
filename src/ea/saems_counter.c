/* A fragment of saems.c (0x80155228): the AEMS range-trigger, envelope and
   counter components' updates. The range trigger fires (output, hold count
   and armed flag set to 1) when its input enters the on range while
   disarmed, counts the hold down, and re-arms when the input enters the off
   range; otherwise its output is 0. The envelope restarts on trigger 1,
   jumps to its release point on trigger 3, and otherwise advances through
   its (duration, level) points by the sample rate, interpolating in 32.32
   fixed point; trigger 2 holds the output. For the counter, an input inside the defined range sets the
   output directly; otherwise a rising trigger (input 0 positive while the
   previous trigger was zero) steps the output up or down and wraps it between
   the limits; the trigger is remembered. The file name is this project's; the
   original record is saems.c (src/ea/saems.c holds the multiplexer onward), and
   AEMSI_updaterandom between them is not reconstructed. AEMSCOMPDYNAMIC and
   MODULE are named by the mangled symbols; the component and definition
   layouts are inferred, as in saems.c. */
struct MODULE;

struct AEMSCOMPDEFVIEW {
    unsigned char unknown00[12];
    int param0;
    int param1;
};

struct AEMSTABLEDEFVIEW {
    unsigned char unknown00[12];
    short* table;
    short count;
    short min;
    short max;
};

struct AEMSTABLE2DEFVIEW {
    unsigned char unknown00[12];
    void* table;
    int count;
    int min;
    int max;
    int width;
};

struct AEMSCOUNTERDEFVIEW {
    unsigned char unknown00[12];
    int min;
    int max;
    unsigned char up;
};

struct AEMSRANGETRIGDEFVIEW {
    unsigned char unknown00[12];
    int onMin;
    int onMax;
    int offMin;
    int offMax;
};

struct AEMSENVPOINTVIEW {
    int duration;
    int level;
};

struct AEMSENVDEFVIEW {
    unsigned char unknown00[12];
    AEMSENVPOINTVIEW* points;
    short count;
    short release;
    int initial;
};

struct AEMSRANDOMDEFVIEW {
    unsigned char unknown00[12];
    int min;
    int max;
    unsigned char* weights;
    unsigned char mode;
};

struct AEMSSTATEGENDEFVIEW {
    unsigned char unknown00[12];
    int count;
    short* values;
};

union AEMSOUTPUTVIEW {
    int value;
    int* values;
};

union AEMSSTATEVIEW {
    int value;
    short* samples;
    unsigned short* counts;
};

struct AEMSCOMPDYNAMIC {
    void* def;
    unsigned char unknown04[8];
    int* in;
    AEMSOUTPUTVIEW out;
    AEMSSTATEVIEW state0;
    union {
        int value;
        short* samples;
        int* values;
        short round;
        struct {
            short stage;
            short trigger;
        } env;
    } state1;
    union {
        int value;
        short* samples;
    } state2;
};

struct SNDAEMSVIEW {
    int unknown00;
    int rate;
    unsigned char unknown08[35];
    unsigned char handlerCount;
    void (*handlers[6])(int, int*);
};

extern SNDAEMSVIEW sndaems;
extern void (*SNDAEMS_beginevent)(int*);

void SNDAEMSI_destroyinstance(MODULE*, int);
unsigned int iSNDrandom();

void AEMSI_updaterangetrig(AEMSCOMPDYNAMIC* comp, MODULE*) {
    int in;
    AEMSRANGETRIGDEFVIEW* def;

    def = (AEMSRANGETRIGDEFVIEW*)comp->def;
    in = comp->in[0];

    if (in >= def->onMin && in <= def->onMax && comp->state1.value == 0) {
        comp->out.value = comp->state0.value = comp->state1.value = 1;
        return;
    }
    if (comp->state0.value > 0) {
        comp->state0.value--;
        return;
    }
    if (in >= def->offMin && in <= def->offMax && comp->state1.value == 1) {
        comp->state1.value = 0;
        return;
    }
    comp->out.value = 0;
}

void AEMSI_updateenvelope(AEMSCOMPDYNAMIC* comp, MODULE*) {
    AEMSENVDEFVIEW* def = (AEMSENVDEFVIEW*)comp->def;
    int trigger = comp->in[0];

    if (trigger == 1 && comp->state1.env.trigger == 0) {
        comp->state0.value = 0;
        comp->state1.env.stage = 0;
        comp->out.value = def->initial;
        comp->state2.value = def->initial;
    } else if (trigger == 3 && comp->state1.env.trigger != 3 && def->release - 1 > comp->state1.env.stage) {
        comp->state0.value = 0;
        comp->state1.env.stage = def->release - 1;
        comp->out.value = comp->state2.value = def->points[comp->state1.env.stage - 1].level;
    } else if ((trigger == 3 || trigger == 1) && comp->state1.env.stage < def->count) {
        comp->state0.value += sndaems.rate;
        if (comp->state0.value >= def->points[comp->state1.env.stage].duration) {
            comp->state0.value = 0;
            comp->state2.value = def->points[comp->state1.env.stage].level;
            comp->state1.env.stage++;
            if (comp->state1.env.stage >= def->count)
                comp->out.value = 0;
            else
                comp->out.value = comp->state2.value;
        } else {
            long long fraction = ((long long)comp->state0.value << 32) / def->points[comp->state1.env.stage].duration;
            comp->out.value = (fraction * (def->points[comp->state1.env.stage].level - comp->state2.value)) >> 32;
            comp->out.value += comp->state2.value;
        }
    } else if (trigger != 2) {
        comp->out.value = 0;
    }
    comp->state1.env.trigger = comp->in[0];
}

void AEMSI_updatecounter(AEMSCOMPDYNAMIC* comp, MODULE*) {
    AEMSCOUNTERDEFVIEW* def = (AEMSCOUNTERDEFVIEW*)comp->def;
    if (comp->in[1] >= def->min && comp->in[1] <= def->max) {
        comp->out.value = comp->in[1];
    } else if (comp->in[0] > 0 && comp->state0.value == 0) {
        if (def->up) {
            comp->out.value++;
            if (comp->out.value > def->max)
                comp->out.value = def->min;
        } else {
            comp->out.value--;
            if (comp->out.value < def->min)
                comp->out.value = def->max;
        }
    }
    comp->state0.value = comp->in[0];
}
