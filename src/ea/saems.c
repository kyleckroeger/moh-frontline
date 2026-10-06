/* The AEMS module updaters from the multiplexer to the modulo component: each
   reads the component's inputs and writes its output, clamping or guarding
   against division by zero. AEMSCOMPDYNAMIC and MODULE are named by the
   mangled symbols; the component layout and the static definition it points
   to are inferred views (the definition's two parameters are a count and a
   limit for some components and a minimum or maximum for others). */
struct MODULE;

struct AEMSCOMPDEFVIEW {
    unsigned char unknown00[12];
    int param0;
    int param1;
};

union AEMSOUTPUTVIEW {
    int value;
    int* values;
};

struct AEMSCOMPDYNAMIC {
    AEMSCOMPDEFVIEW* def;
    unsigned char unknown04[8];
    int* in;
    AEMSOUTPUTVIEW out;
};

void AEMSI_updatemux(AEMSCOMPDYNAMIC* comp, MODULE*) {
    int* in = comp->in;
    if (in[0] > 0)
        comp->out.value = in[in[0]];
    else
        comp->out.value = 0;
}

void AEMSI_updatedemux(AEMSCOMPDYNAMIC* comp, MODULE*) {
    AEMSCOMPDEFVIEW* def = comp->def;
    int i;
    for (i = 0; i < def->param0; i++) {
        if (i == comp->in[0] - 1)
            comp->out.values[i] = comp->in[1];
        else
            comp->out.values[i] = 0;
    }
}

void AEMSI_updateadd(AEMSCOMPDYNAMIC* comp, MODULE*) {
    AEMSCOMPDEFVIEW* def = comp->def;
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
    if (result < comp->def->param0)
        comp->out.value = comp->def->param0;
    else
        comp->out.value = result;
}

void AEMSI_updatescale(AEMSCOMPDYNAMIC* comp, MODULE*) {
    AEMSCOMPDEFVIEW* def = comp->def;
    int i;
    comp->out.value = comp->in[0];
    for (i = 1; i < def->param1; i++)
        comp->out.value = ((long long)comp->out.value * comp->in[i]) / def->param0;
}

void AEMSI_updatemultiply(AEMSCOMPDYNAMIC* comp, MODULE*) {
    int result = comp->in[0] * comp->in[1];
    if (result > comp->def->param0)
        comp->out.value = comp->def->param0;
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
