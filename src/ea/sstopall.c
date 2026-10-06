// SNDstopall: stop every voice. sndgs's layout is not known; the voice table
// (128-byte entries starting with the handle) and voice count are read through
// inferred offsets.
extern "C" {
extern char sndgs[];
void SNDSYS_entercritical(void);
void SNDSYS_leavecritical(void);
int SNDstop(int);
}

extern "C" void SNDstopall(void) {
    int i;

    SNDSYS_entercritical();
    for (i = 0; i < *(short*)(sndgs + 368); i++)
        SNDstop(*(int*)(*(char**)(sndgs + 468) + i * 128));
    SNDSYS_leavecritical();
}
