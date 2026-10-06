/* A fragment of saems.c: the AEMS counter component's update
   (AEMSI_updatecounter, 0x801554a4). An input inside the defined range sets the
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
