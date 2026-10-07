/* A fragment of the sound driver (snddrv.c, 0x8016c3e8): the current frame
   of a voice (for an AX voice, its playback address minus the start, scaled
   by 7/8 for type 18 samples; otherwise the software mixer's frame).
   SNDPLATFORM_stop after it is drafted in scratch/path/sdrv_stop_wip.cpp.
   sndgs and snddrv are named by their symbols; their views, the voice
   records and the AX voice block's members are inferred. */
struct _AXVPB;

extern "C" {
extern char sndgs[];
int MIX_getframe(int);
}

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
    unsigned char unknown0c[32];
    _AXVPB* axvoice;
};

struct SNDDRVVIEW {
    SNDDRVVOICE voices[61];
};

extern SNDDRVVIEW snddrv;

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
