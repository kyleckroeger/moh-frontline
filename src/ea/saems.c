/* The AEMS module updaters from the multiplexer to the event sender: each
   reads the component's inputs and writes its output (arithmetic with
   clamping, table lookups, delays, extremes), destroys the instance or sends
   an event when the inputs change. AEMSCOMPDYNAMIC and MODULE are named by
   the mangled symbols; the component layout, the per-component static
   definitions it points to, the sndaems view and the event buffer's size
   (from the stack frame) are inferred. */
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
    } state1;
    short* read;
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
void AEMSI_updatemux(AEMSCOMPDYNAMIC* comp, MODULE*) {
    int* in = comp->in;
    if (in[0] > 0)
        comp->out.value = in[in[0]];
    else
        comp->out.value = 0;
}

void AEMSI_updatedemux(AEMSCOMPDYNAMIC* comp, MODULE*) {
    AEMSCOMPDEFVIEW* def = (AEMSCOMPDEFVIEW*)comp->def;
    int i;
    for (i = 0; i < def->param0; i++) {
        if (i == comp->in[0] - 1)
            comp->out.values[i] = comp->in[1];
        else
            comp->out.values[i] = 0;
    }
}

void AEMSI_updateadd(AEMSCOMPDYNAMIC* comp, MODULE*) {
    AEMSCOMPDEFVIEW* def = (AEMSCOMPDEFVIEW*)comp->def;
    int sum = 0;
    int i;
    for (i = 0; i < def->param0; i++)
        sum += comp->in[i];
    if (sum > def->param1)
        comp->out.value = def->param1;
    else
        comp->out.value = sum;
}

void AEMSI_updatesubtract(AEMSCOMPDYNAMIC* comp, MODULE*) {
    int result = comp->in[0] - comp->in[1];
    if (result < ((AEMSCOMPDEFVIEW*)comp->def)->param0)
        comp->out.value = ((AEMSCOMPDEFVIEW*)comp->def)->param0;
    else
        comp->out.value = result;
}

void AEMSI_updatescale(AEMSCOMPDYNAMIC* comp, MODULE*) {
    AEMSCOMPDEFVIEW* def = (AEMSCOMPDEFVIEW*)comp->def;
    int i;
    comp->out.value = comp->in[0];
    for (i = 1; i < def->param1; i++)
        comp->out.value = ((long long)comp->out.value * comp->in[i]) / def->param0;
}

void AEMSI_updatemultiply(AEMSCOMPDYNAMIC* comp, MODULE*) {
    int result = comp->in[0] * comp->in[1];
    if (result > ((AEMSCOMPDEFVIEW*)comp->def)->param0)
        comp->out.value = ((AEMSCOMPDEFVIEW*)comp->def)->param0;
    else
        comp->out.value = result;
}

void AEMSI_updatedivide(AEMSCOMPDYNAMIC* comp, MODULE*) {
    if (comp->in[1] > 0)
        comp->out.value = comp->in[0] / comp->in[1];
    else
        comp->out.value = 0;
}

void AEMSI_updatemodulo(AEMSCOMPDYNAMIC* comp, MODULE*) {
    comp->out.value = comp->in[0] % comp->in[1];
}

void AEMSI_updatetable(AEMSCOMPDYNAMIC* comp, MODULE*) {
    AEMSTABLEDEFVIEW* def = (AEMSTABLEDEFVIEW*)comp->def;
    comp->out.value = comp->in[0];
    if (comp->out.value < def->min)
        comp->out.value = def->min;
    else if (comp->out.value > def->max)
        comp->out.value = def->max;
    comp->out.value = ((def->count - 1) * (comp->out.value - def->min)) / (def->max - def->min);
    comp->out.value = def->table[comp->out.value];
}

void AEMSI_updatetable2(AEMSCOMPDYNAMIC* comp, MODULE*) {
    AEMSTABLE2DEFVIEW* def = (AEMSTABLE2DEFVIEW*)comp->def;
    comp->out.value = comp->in[0];
    if (comp->out.value < def->min)
        comp->out.value = def->min;
    else if (comp->out.value > def->max)
        comp->out.value = def->max;
    comp->out.value = (def->count * (comp->out.value - def->min)) / (def->max - def->min + 1);
    if (def->width == 4)
        comp->out.value = ((int*)def->table)[comp->out.value];
    else if (def->width == 2)
        comp->out.value = ((short*)def->table)[comp->out.value];
    else {
        signed char* bytes = (signed char*)def->table;
        comp->out.value = bytes[comp->out.value];
    }
}

void AEMSI_updatedelaytrig(AEMSCOMPDYNAMIC* comp, MODULE*) {
    AEMSCOMPDEFVIEW* def = (AEMSCOMPDEFVIEW*)comp->def;
    if (comp->in[0] > 0 && comp->state1.value == 0)
        comp->state0.value = 0;
    if ((comp->state0.value >= comp->in[1] || comp->state0.value > def->param0) && comp->out.value == 0) {
        comp->out.value = 1;
    } else if (comp->state0.value > comp->in[1] || comp->state0.value > def->param0) {
        comp->out.value = 0;
        comp->state0.value = -1;
    } else {
        comp->out.value = 0;
    }
    comp->state1.value = comp->in[0];
    if (comp->state0.value >= 0)
        comp->state0.value += sndaems.rate;
}

void AEMSI_updatedelayline(AEMSCOMPDYNAMIC* comp, MODULE*) {
    AEMSCOMPDEFVIEW* def = (AEMSCOMPDEFVIEW*)comp->def;
    comp->state1.samples++;
    if (comp->state1.samples >= comp->state0.samples + def->param0 / sndaems.rate)
        comp->state1.samples = comp->state0.samples;
    *comp->state1.samples = comp->in[0];
    if (comp->in[1] / sndaems.rate > def->param0 / sndaems.rate - 1)
        comp->read = comp->state1.samples - def->param0 / sndaems.rate + 1;
    else
        comp->read = comp->state1.samples - comp->in[1] / sndaems.rate;
    if (comp->read < comp->state0.samples)
        comp->read += def->param0 / sndaems.rate;
    comp->out.value = *comp->read;
}

void AEMSI_updatewarp(AEMSCOMPDYNAMIC* comp, MODULE*) {
    comp->out.value = comp->in[0];
}

void AEMSI_updatemax(AEMSCOMPDYNAMIC* comp, MODULE*) {
    int result = 0;
    int i;
    for (i = 0; i < ((AEMSCOMPDEFVIEW*)comp->def)->param0; i++) {
        if (comp->in[i] > result)
            result = comp->in[i];
    }
    comp->out.value = result;
}

void AEMSI_updatemin(AEMSCOMPDYNAMIC* comp, MODULE*) {
    int result = 0x7fffffff;
    int i;
    for (i = 0; i < ((AEMSCOMPDEFVIEW*)comp->def)->param0; i++) {
        if (comp->in[i] < result)
            result = comp->in[i];
    }
    comp->out.value = result;
}

void AEMSI_updatestategen(AEMSCOMPDYNAMIC* comp, MODULE*) {
    AEMSSTATEGENDEFVIEW* def = (AEMSSTATEGENDEFVIEW*)comp->def;
    int i;
    for (i = 0; i < def->count; i++) {
        if (comp->state0.samples[i] <= 0 && comp->in[i] > 0)
            comp->out.value = def->values[i];
        comp->state0.samples[i] = comp->in[i];
    }
}

void SNDAEMSI_updatedestroy(AEMSCOMPDYNAMIC* comp, MODULE* module) {
    if (comp->in[0] > 0)
        SNDAEMSI_destroyinstance(module, comp->state0.value);
}

void SNDAEMSI_updatesend(AEMSCOMPDYNAMIC* comp, MODULE*) {
    AEMSCOMPDEFVIEW* def = (AEMSCOMPDEFVIEW*)comp->def;
    int event[18];
    int i;
    int changed = 0;
    for (i = 0; i < def->param0; i++) {
        if (comp->state1.values[i] != comp->in[i])
            changed = 1;
        comp->state1.values[i] = comp->in[i];
    }
    if (changed == 1) {
        event[0] = def->param1;
        for (i = 0; i < def->param0; i++)
            event[i + 1] = comp->in[i];
        SNDAEMS_beginevent(event);
        for (i = 0; i < sndaems.handlerCount; i++)
            sndaems.handlers[i](comp->state0.value, event);
    }
}
