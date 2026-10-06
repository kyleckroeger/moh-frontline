// REAL pad interface on the platform layer. gPadStatus holds one 12-byte
// status record per controller (the size of Dolphin's PADStatus).
struct PADStatus {
    unsigned short button;
    signed char stickX;
    signed char stickY;
    signed char substickX;
    signed char substickY;
    unsigned char triggerLeft;
    unsigned char triggerRight;
    unsigned char analogA;
    unsigned char analogB;
    signed char err;
};

extern PADStatus gPadStatus[];
void iPAD_init(void);
void iPAD_update(void);

extern "C" {
void PAD_init(void) {
    iPAD_init();
}

PADStatus* PAD_getdataptr(int pad) {
    return &gPadStatus[pad];
}

void PAD_update(void) {
    iPAD_update();
}
}
