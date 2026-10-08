// Hooks the sound system into the REAL runtime: a system task that services
// the sound system, an abort-message hook and an exit handler; then
// SNDSYSI_100hzserver, the sound system's 100 Hz tick: it counts the tick,
// serves the driver, calls the registered clients and steps every playing
// voice's pitch and volume LFO phases, volume fade (stopping the voice when it
// fades below zero) and envelope (stopping it after the last stage), then
// recomputes and applies the volume when it changed. Only the abort-message
// hook at sndgs +0xc4 is established; the other sndgs members and the voice
// and envelope views are inferred.
/* inferred: an envelope stage (target level, then step count) */
struct SNDENVSTAGEVIEW {
    int count;
    int level;
};

/* inferred: a voice record (128 bytes) */
struct SNDVOICEVIEW {
    int handle;
    unsigned char unknown04[40];
    int fadeRate;
    int fadeTarget;
    int fadeVolume;
    int envRate;
    int envLevel;
    int envCount;
    unsigned char unknown44[12];
    signed char envStages;
    signed char envStage;
    unsigned char unknown52[7];
    unsigned char volLfoLength;
    unsigned char pitchLfoLength;
    unsigned char volLfoPhase;
    unsigned char pitchLfoPhase;
    char status;
    unsigned char unknown5e[2];
    SNDENVSTAGEVIEW* envelope;
    unsigned char unknown64[8];
    void* volLfo;
    void* pitchLfo;
    unsigned char unknown74[6];
    short value7a;
    unsigned char unknown7c[4];
};

struct SNDGSView {
    unsigned char unknown00[196];
    void (*abortmsg)(char*);
    unsigned char unknown0c8[164];
    signed char clientCount;
    unsigned char unknown16d[3];
    short voiceCount;
    unsigned char unknown172[2];
    int ticks;
    void (*clients[23])(void);
    SNDVOICEVIEW* voices;
};

extern SNDGSView sndgs;

void iSNDserve();
void iSNDcalcpitch(int);
void iSNDcalcvol(int);
void SNDPLATFORM_setpitch(int);
void SNDPLATFORM_setvol(int);

extern "C" {
void SNDstop(int);
void SNDSYS_service(void);
void SYNCTASK_add(int (*)(int, int), int, int);
void REAL_addexit(void (*)(void));
}
void SNDREAL_exithandler();

int SNDREAL_systemtask(int, int) {
    SNDSYS_service();
    return 0;
}

void SNDREAL_abortmsg(char*) {
}

extern "C" int SNDSYS_vectortoreal(void) {
    static int systaskadded = 0;

    sndgs.abortmsg = SNDREAL_abortmsg;
    if (!systaskadded) {
        SYNCTASK_add(SNDREAL_systemtask, 0, 1);
        systaskadded = 1;
    }
    REAL_addexit(SNDREAL_exithandler);
    return 0;
}

void SNDSYSI_100hzserver() {
    int i;
    int changed;

    sndgs.ticks++;
    iSNDserve();
    for (i = 0; i < sndgs.clientCount; i++)
        sndgs.clients[i]();
    for (i = 0; i < sndgs.voiceCount; i++) {
        SNDVOICEVIEW* voice = &sndgs.voices[i];
        if (voice->status != 1)
            continue;
        if (voice->handle < 0)
            continue;
        if (voice->pitchLfo) {
            voice->pitchLfoPhase++;
            if (voice->pitchLfoPhase >= voice->pitchLfoLength)
                voice->pitchLfoPhase = 0;
            voice->value7a = 0;
            iSNDcalcpitch(i);
            SNDPLATFORM_setpitch(i);
        }
        changed = 0;
        if (voice->volLfo) {
            changed = 1;
            voice->volLfoPhase++;
            if (voice->volLfoPhase >= voice->volLfoLength)
                voice->volLfoPhase = 0;
        }
        if (voice->fadeRate) {
            changed = 1;
            voice->fadeVolume += voice->fadeRate;
            if (voice->fadeRate < 0) {
                if (voice->fadeVolume <= voice->fadeTarget) {
                    voice->fadeVolume = voice->fadeTarget;
                    voice->fadeRate = 0;
                }
            } else {
                if (voice->fadeVolume >= voice->fadeTarget) {
                    voice->fadeVolume = voice->fadeTarget;
                    voice->fadeRate = 0;
                }
            }
            if (voice->fadeVolume < 0) {
                SNDstop(voice->handle);
                continue;
            }
        }
        voice->envCount--;
        if (voice->envRate) {
            changed = 1;
            voice->envLevel += voice->envRate;
        }
        if (voice->envCount == 0) {
            SNDENVSTAGEVIEW* stage;
            voice->envStage++;
            if (voice->envStage >= voice->envStages) {
                SNDstop(voice->handle);
                continue;
            }
            stage = &voice->envelope[voice->envStage];
            voice->envCount = stage->count;
            if (voice->envCount < 0)
                voice->envCount = 0x7FFFFFFF;
            voice->envRate = ((stage->level << 16) - voice->envLevel) / voice->envCount;
        }
        if (changed) {
            iSNDcalcvol(i);
            if (voice->handle >= 0)
                SNDPLATFORM_setvol(i);
        }
    }
}
