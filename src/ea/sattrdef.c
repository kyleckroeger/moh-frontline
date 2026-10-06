// SND_attrsetdef: default sound attributes. The record's members are
// inferred from offsets and are not original.
struct SNDATTR {
    short short0;
    signed char byte2;
    signed char byte3;
    signed char byte4;
    signed char byte5;
    signed char byte6;
    signed char byte7;
    short short8;
    short shortA;
    short shortC[4];
    int int14[4];
    int int24[4];
    int int34[4];
    int int44[4];
};

extern "C" int SND_attrsetdef(SNDATTR* attr) {
    int i;

    attr->byte2 = 0;
    attr->short0 = 0;
    attr->byte3 = 127;
    attr->byte4 = 64;
    attr->byte5 = 0;
    attr->byte6 = 0;
    attr->byte7 = 2;
    attr->short8 = 512;
    for (i = 0; i < 4; i++) {
        attr->int34[i] = 0;
        attr->int44[i] = 0;
    }
    for (i = 0; i < 4; i++) {
        attr->shortC[i] = 0;
        attr->int14[i] = 0;
    }
    return 0;
}
