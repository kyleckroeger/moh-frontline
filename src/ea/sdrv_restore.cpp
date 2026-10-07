/* A fragment of the sound driver (snddrv.c, 0x8016ac84): restoring the
   platform layer - the installed reset hook is called, the AUX A callback
   removed, the software mixer's buffers freed and the mixer restored (when it
   was set up), ARAM restored, pending critical sections drained, the driver
   thread cancelled, the 100 Hz clients removed and AX shut down. sndgs and
   snddrv are named by their symbols; their views are inferred. */
struct OSThread;

extern "C" {
extern char sndgs[];
extern void (*MIXrestorefn)(void);
void AXRegisterAuxACallback(void*, void*);
void SNDSYS_entercritical(void);
void SNDSYS_leavecritical(void);
void OSCancelThread(OSThread*);
void SNDSYS_remove100hzclient(void (*)(void));
void AXQuit(void);
}
void SNDMEMI_free(void*);
void SNDARAM_restore(void);
void SNDDRV_freenextframe(void);
void SNDDRV_DSPMixerUpdate(void);

/* inferred: an OS thread block */
struct OSThread {
    unsigned char unknown000[792];
};

struct SNDDRVVIEW {
    unsigned char unknown0000[3708];
    void (*reset)(int);
    unsigned char unknown0e80[5448 - 3712];
    OSThread thread;
    unsigned char unknown1860[0xaac0 - 5448 - 792];
    signed char mixerinited;
    unsigned char unknownaac1[7];
    void* mixalloc[4];
};

extern SNDDRVVIEW snddrv;

/* inferred: frees the software mixer's buffers */
static inline void freemixbuffers(void) {
    int i;

    SNDSYS_entercritical();
    for (i = 0; i < *(unsigned char*)(sndgs + 72); i++)
        SNDMEMI_free(snddrv.mixalloc[i]);
    SNDSYS_leavecritical();
}

int SNDPLATFORM_restore(void) {
    if (snddrv.reset)
        snddrv.reset(4);
    AXRegisterAuxACallback(0, 0);
    if (snddrv.mixerinited) {
        snddrv.mixerinited = 0;
        freemixbuffers();
        MIXrestorefn();
    }
    SNDARAM_restore();
    while (*(signed char*)(sndgs + 363))
        SNDSYS_leavecritical();
    OSCancelThread(&snddrv.thread);
    SNDSYS_remove100hzclient(SNDDRV_freenextframe);
    SNDSYS_remove100hzclient(SNDDRV_DSPMixerUpdate);
    AXQuit();
    return 0;
}
