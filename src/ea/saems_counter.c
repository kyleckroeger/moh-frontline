/* A fragment of saems.c (0x80154e54): the AEMS input-parameter and static
   components (no update), the trigger (a set output fires once, then clears),
   the oscillator (a step from the rate over the input period in mode 2; a
   16-bit phase driving sine, square, saw or triangle waves, unipolar or
   bipolar), the player (clamps its inputs, picks a sound from its table on a
   rising play input and hands it to the stream, player or bank hook of
   sndaems by the sound id's top bits), and the range-trigger, envelope and
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

struct AEMSOSCDEFVIEW {
    unsigned char unknown00[12];
    short waveform;
    short polarity;
};

struct AEMSPLAYERTABLEVIEW {
    int* sounds;
    short count;
};

struct AEMSPLAYERDEFVIEW {
    unsigned char unknown00[12];
    AEMSPLAYERTABLEVIEW* table;
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
    unsigned short step;
};

struct AEMSCOMPDYNAMIC {
    void* def;
    signed char mode;
    unsigned char unknown05[7];
    int* in;
    AEMSOUTPUTVIEW out;
    AEMSSTATEVIEW state0;
    union {
        int value;
        short* samples;
        int* values;
        short round;
        unsigned int phase;
        struct {
            short stage;
            short trigger;
        } env;
    } state1;
    union {
        int value;
        short* samples;
    } state2;
    unsigned char unknown20[12];
    unsigned short sound;
    unsigned char playing;
};

struct SNDAEMSVIEW {
    int unknown00;
    int rate;
    unsigned char unknown08[16];
    void (*playerUpdate)(AEMSCOMPDYNAMIC*);
    unsigned char unknown1c[4];
    void (*bankUpdate)(AEMSCOMPDYNAMIC*);
    void (*streamUpdate)(AEMSCOMPDYNAMIC*);
    unsigned char unknown28[3];
    unsigned char handlerCount;
    void (*handlers[6])(int, int*);
};

extern SNDAEMSVIEW sndaems;
extern void (*SNDAEMS_beginevent)(int*);

void SNDAEMSI_destroyinstance(MODULE*, int);
unsigned int iSNDrandom();
int iSNDsin(int);

void AEMSI_updateinparam(AEMSCOMPDYNAMIC*, MODULE*) {
}

void AEMSI_updatetrigger(AEMSCOMPDYNAMIC* comp, MODULE*) {
    if (comp->out.value >= 1) {
        if (comp->state0.value == 0)
            comp->state0.value = 1;
        else
            comp->out.value = comp->state0.value = 0;
    }
}

void AEMSI_updatestatic(AEMSCOMPDYNAMIC*, MODULE*) {
}

void AEMSI_updateoscillator(AEMSCOMPDYNAMIC* comp, MODULE*) {
    AEMSOSCDEFVIEW* def = (AEMSOSCDEFVIEW*)comp->def;

    if (comp->mode == 2) {
        int period = comp->in[0];
        int step;

        if (period <= 0)
            period = 1;
        step = (sndaems.rate << 16) / period;
        if (step > 0xFFFF)
            step = 0xFFFF;
        comp->state0.step = step;
    }
    comp->state1.phase += comp->state0.step;
    if (comp->state1.phase >= 0x10000)
        comp->state1.phase -= 0x10000;
    if (def->waveform == 0) {
        comp->out.value = (comp->in[1] * iSNDsin(comp->state1.phase >> 6)) >> 16;
        if (def->polarity == 1) {
            comp->out.value += comp->in[1];
            comp->out.value >>= 1;
        }
    } else if (def->waveform == 1) {
        if (comp->state1.phase >= 0x8000)
            comp->out.value = comp->in[1];
        else if (def->polarity == 1)
            comp->out.value = 0;
        else
            comp->out.value = -comp->in[1];
    } else if (def->waveform == 2 && def->polarity == 1) {
        comp->out.value = (comp->state1.phase * comp->in[1]) >> 16;
    } else if (def->waveform == 2) {
        comp->out.value = (comp->state1.phase * comp->in[1]) >> 15;
        comp->out.value -= comp->in[1];
    } else if (def->waveform == 3 && def->polarity == 0) {
        if (comp->state1.phase < 0x8000)
            comp->out.value = (comp->state1.phase * comp->in[1]) >> 14;
        else
            comp->out.value = ((0x10000 - comp->state1.phase) * comp->in[1]) >> 14;
        comp->out.value -= comp->in[1];
    } else if (def->waveform == 3) {
        if (comp->state1.phase < 0x8000)
            comp->out.value = (comp->state1.phase * comp->in[1]) >> 15;
        else
            comp->out.value = ((0x10000 - comp->state1.phase) * comp->in[1]) >> 15;
    }
}

void SNDAEMSI_updateplayer(AEMSCOMPDYNAMIC* comp, MODULE*) {
    AEMSPLAYERDEFVIEW* def = (AEMSPLAYERDEFVIEW*)comp->def;

    if (comp->in[0] > 0xFFFF)
        comp->in[0] = 0xFFFF;
    else if (comp->in[0] < 0)
        comp->in[0] = 0;
    if (comp->in[2] > 0x7FFF)
        comp->in[2] = 0x7FFF;
    else if (comp->in[2] < 0)
        comp->in[2] = 0;
    if (comp->playing != comp->in[7] && comp->in[7] == 1 && comp->playing == 0) {
        AEMSPLAYERTABLEVIEW* table = def->table;
        int index = comp->in[6];

        if (index >= table->count)
            index = table->count - 1;
        comp->sound = table->sounds[index];
    }
    if (comp->sound >= 0xC000)
        sndaems.streamUpdate(comp);
    else if (comp->sound >= 0x8000)
        sndaems.playerUpdate(comp);
    else if (comp->sound >= 0x4000)
        sndaems.bankUpdate(comp);
}

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
