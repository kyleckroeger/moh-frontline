/* A fragment of the sound driver (snddrv.c, 0x8016c364): the high-pass
   setting of a mixer voice's channels, and the current frame
   of a voice (for an AX voice, its playback address minus the start, scaled
   by 7/8 for type 18 samples; otherwise the software mixer's frame).
   SNDPLATFORM_stop stops a voice: an AX voice's packet player is detached
   (through an inline helper) and its AX voices stopped and released; a mixer
   voice's channels are stopped and freed.
   sndgs and snddrv are named by their symbols; their views, the voice
   records and the AX voice block's members are inferred. */
struct _AXVPB;

extern "C" {
extern char sndgs[];
extern void (*MIXstopfn)(int);
int MIX_getframe(int);
void AXSetVoiceState(_AXVPB*, unsigned short);
}
void SNDDRV_freeaxvoice(int, int);
void SNDVOICEI_free(int);
extern "C" void MIX_sethighpass(int, int);

/* inferred: an AX voice parameter block, as far as it is read here */
struct _AXVPB {
    unsigned char unknown000[434];
    unsigned short currentHi;
    unsigned short currentLo;
};

/* inferred: a sound voice record (128 bytes) */
struct SNDVOICEVIEW {
    unsigned char unknown00[4];
    short channels[4];
    unsigned char unknown0c[18];
    unsigned char type;
    unsigned char count;
    unsigned short flags;
    unsigned char unknown22[94];
};

/* inferred: the driver's per-channel record (48 bytes) */
struct SNDDRVVOICE {
    unsigned char unknown00[8];
    int start;
    signed char packet;
    unsigned char unknown0d[7];
    unsigned char allocated;
    unsigned char unknown15[23];
    _AXVPB* axvoice;
};

/* inferred: the packet player's platform record */
struct SNDDRVPACKETVIEW {
    unsigned char unknown0000[4812];
    signed char state;
};

struct SNDDRVVIEW {
    SNDDRVVOICE voices[61];
    unsigned char unknownb70[3716 - 61 * 48];
    SNDDRVPACKETVIEW* packets[1];
};

extern SNDDRVVIEW snddrv;

void SNDPLATFORM_highpass(int voice, int value) {
    SNDVOICEVIEW* sound = &(*(SNDVOICEVIEW**)(sndgs + 468))[voice];
    int i;

    if (sound->flags & 4) {
        for (i = 0; i < sound->count; i++)
            MIX_sethighpass(sound->channels[i] - *(unsigned char*)(sndgs + 51), value);
    }
}

int SNDPLATFORM_getcurframe(int voice) {
    SNDDRVVOICE* driver = &snddrv.voices[voice];
    SNDVOICEVIEW* sound = &(*(SNDVOICEVIEW**)(sndgs + 468))[voice];

    if (sound->flags & 0x200) {
        int position;

        if (!driver->axvoice)
            return -8;
        position = (driver->axvoice->currentHi << 16) + driver->axvoice->currentLo;
        if (sound->type == 18)
            return ((position - driver->start) * 7) >> 3;
        return position - driver->start;
    }
    return MIX_getframe(voice - *(unsigned char*)(sndgs + 51));
}

/* inferred: detaches a voice from its packet player */
static inline void stoppacket(SNDDRVVOICE* driver) {
    if (driver->packet >= 0) {
        snddrv.packets[driver->packet]->state = -1;
        driver->packet = -1;
    }
}

int SNDPLATFORM_stop(int voice) {
    SNDVOICEVIEW* sound = &(*(SNDVOICEVIEW**)(sndgs + 468))[voice];
    SNDDRVVOICE* driver = &snddrv.voices[voice];
    int i;

    if (sound->flags & 0x200) {
        stoppacket(driver);
        for (i = 0; i < sound->count; i++) {
            if (!snddrv.voices[sound->channels[i]].axvoice)
                return -8;
            AXSetVoiceState(snddrv.voices[sound->channels[i]].axvoice, 0);
            if (snddrv.voices[sound->channels[i]].allocated) {
                SNDDRV_freeaxvoice(sound->channels[i], 0);
                snddrv.voices[sound->channels[i]].allocated = 0;
            }
        }
    } else {
        for (i = 0; i < sound->count; i++) {
            MIXstopfn(sound->channels[i] - *(unsigned char*)(sndgs + 51));
            SNDVOICEI_free(sound->channels[i]);
        }
    }
    return 0;
}
