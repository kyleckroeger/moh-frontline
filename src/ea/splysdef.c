// SNDplaysetdef: default play options. SNDPLAYOPTS is named by mangled
// symbols; its members are inferred from offsets and are not original.
struct SNDPLAYOPTS {
    unsigned char byte0;
    unsigned char byte1;
    unsigned char byte2;
    unsigned char byte3;
    unsigned char byte4;
    unsigned char byte5;
    short short6;
    short short8;
    short shortA;
    short shortC;
    short shortE;
    short short10;
    short short12;
    unsigned short short14;
    short short16;
};

extern "C" int SNDplaysetdef(SNDPLAYOPTS* opts) {
    opts->byte2 = 60;
    opts->shortC = 4096;
    opts->shortE = 4096;
    opts->short10 = 4096;
    opts->short12 = 0;
    opts->short14 = 0xffff;
    opts->short16 = 0;
    opts->byte3 = 127;
    opts->byte0 = 127;
    opts->byte4 = 127;
    opts->byte1 = 64;
    opts->byte5 = 127;
    opts->short8 = 0;
    opts->shortA = 0;
    return 0;
}
