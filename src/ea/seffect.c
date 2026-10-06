// Effect buses: bus records (20 bytes, two per group: reverb at 476 and the
// second effect at 496 in sndgs), their set-up and master send levels.
// sndgs's layout is not known; the members used here are inferred.
extern "C" char sndgs[];

struct SNDFXBUS {
    short type;
    unsigned char level;
    unsigned char field3;
    int param1;
    int param2;
    char fieldC[8];
};

extern "C" {
void SNDSYS_entercritical(void);
void SNDSYS_leavecritical(void);
int SNDfxlevel(int, int, int);
int SNDfxmasterlevel(int, int);
}
void SNDPLATFORM_fxinit(int, int);

SNDFXBUS* SNDCTRLI_getfxbus(int group, int bus) {
    switch (bus & 0x71c) {
    case 4:
        return (SNDFXBUS*)(sndgs + group * 20 + 476);
    case 512:
        return (SNDFXBUS*)(sndgs + group * 20 + 496);
    }
    return 0;
}

extern "C" int SNDfxinitbus(int bus, int level, int type, int param1, int param2) {
    int group;
    int pending;
    int flag;
    int mask;

    if (!(bus & 0x1f8))
        bus |= 0x1f8;
    mask = 0;
    if (bus & 0x10)
        mask |= 4;
    if (bus & 8)
        mask |= 512;
    group = bus & ~0x1f8;
    pending = mask;
    for (flag = 4; pending != 0; flag <<= 1) {
        if (flag & pending) {
            SNDFXBUS* fx = SNDCTRLI_getfxbus(group, flag);

            fx->type = type;
            fx->param1 = param1;
            fx->param2 = param2;
            SNDPLATFORM_fxinit(group, flag);
            pending &= ~flag;
        }
    }
    if (type == 0)
        level = 0;
    SNDfxmasterlevel(bus, level);
    return 0;
}

extern "C" int SNDfxmasterlevel(int bus, int level) {
    int i;
    int group = bus & ~0x1f8;

    if (!(bus & 0x1f8))
        bus |= 0x1f8;
    if (bus & 0x10)
        ((SNDFXBUS*)(sndgs + 476))[group].level = level;
    if (bus & 8)
        ((SNDFXBUS*)(sndgs + 496))[group].level = level;
    SNDSYS_entercritical();
    for (i = 0; i < *(short*)(sndgs + 368); i++) {
        char* entry = *(char**)(sndgs + 468) + i * 128;

        SNDfxlevel(*(int*)entry, group, (entry + 85)[group]);
    }
    SNDSYS_leavecritical();
    return 0;
}
