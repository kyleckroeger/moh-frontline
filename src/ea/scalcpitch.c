/* iSNDcalcpitch (0x8015e03c; the original has no file record for it) works
   out a voice's pitch - when it is not yet known, the detune plus the pitch-bend
   wheel (through the voice's bend table when it has one) and the LFO
   contribution, converted to a linear factor - and scales it by the voice's
   rate. sndgs is named by its symbol; its view and the voice record (128
   bytes) are inferred. */
extern "C" char sndgs[];

/* inferred: a sound voice record (128 bytes) */
struct SNDVOICEVIEW {
    unsigned char unknown00[88];
    signed char wheel;
    unsigned char unknown59[3];
    unsigned char lfo;
    unsigned char unknown5d[11];
    signed char* wheelTable;
    unsigned char unknown6c[4];
    signed char* lfoTable;
    short lfoDepth;
    short bend;
    short detune;
    unsigned short pitch;
    unsigned short rate;
    unsigned short scaled;
};

int iSNDdetunetolinear(int);

void iSNDcalcpitch(int voice) {
    SNDVOICEVIEW* v = &(*(SNDVOICEVIEW**)(sndgs + 468))[voice];

    if (!v->pitch) {
        int detune = v->detune;
        short bend = v->bend;

        if (bend) {
            int wheel;

            if (v->wheelTable)
                wheel = v->wheelTable[v->wheel];
            else
                wheel = v->wheel;
            detune += ((wheel - 64) * bend) >> 6;
        }
        if (v->lfoTable)
            detune += (v->lfoDepth * (v->lfoTable[v->lfo] - 64)) >> 6;
        v->pitch = iSNDdetunetolinear(detune);
    }
    v->scaled = (v->pitch * v->rate) >> 12;
}
