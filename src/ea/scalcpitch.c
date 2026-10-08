/* sclcptch.c (0x8015df5c): iSNDdetunetolinear converts a detune in cents to
   a linear pitch factor (4096 at no detune: whole octaves double or halve it,
   the remaining cents index the sndcents table), and iSNDcalcpitch works out
   a voice's pitch - when it is not yet known, the detune plus the pitch-bend
   wheel (through the voice's bend table when it has one) and the LFO
   contribution, converted to a linear factor - and scales it by the voice's
   rate. The file is built without automatic inlining (-inline on, as sst.c
   in the same library): iSNDcalcpitch calls iSNDdetunetolinear rather than
   inlining it. sndgs and sndcents (the file's own data, declared extern here)
   are named by their symbols; sndgs's view and the voice record (128 bytes)
   are inferred. */
extern "C" char sndgs[];
extern "C" unsigned char sndcents[];

int iSNDdetunetolinear(int detune) {
    int scale = 4096;
    while (detune >= 1200) {
        scale <<= 1;
        detune -= 1200;
    }
    while (detune <= -1200) {
        scale >>= 1;
        detune += 1200;
    }
    int index = detune * 13981 >> 16;
    if (index < -255)
        index = -255;
    if (index < 0)
        return scale * (sndcents[index + 256] + 256) >> 9;
    return scale * (sndcents[index] + 256) >> 8;
}

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
