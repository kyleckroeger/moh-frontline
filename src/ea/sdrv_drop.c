/* A fragment of the sound driver (snddrv.c, 0x80168fa8): the AX callback for
   a dropped voice releases the AX voices behind every channel of its sound
   voice (or the master's), the dropped one with reason 1 and the others
   with reason 2. sndgs and snddrv are named by their symbols; their views,
   the voice records and the AX block's user index are inferred. */
struct _AXVPB {
    unsigned char unknown00[20];
    int voice;
};

extern "C" char sndgs[];

/* inferred: a sound voice record (128 bytes) */
struct SNDVOICEVIEW {
    unsigned char unknown00[4];
    short channels[4];
    unsigned char unknown0c[19];
    unsigned char count;
    unsigned char unknown20[4];
    short master;
    unsigned char unknown26[90];
};

/* inferred: the driver's per-channel record (48 bytes) */
struct SNDDRVVOICE {
    unsigned char unknown00[20];
    unsigned char allocated;
    unsigned char unknown15[23];
    _AXVPB* axvoice;
};

struct SNDDRVVIEW {
    SNDDRVVOICE voices[61];
};

extern SNDDRVVIEW snddrv;

void SNDDRV_freeaxvoice(int, int);

void SNDDRV_callbackdropvoice(void* data) {
    _AXVPB* dropped = (_AXVPB*)data;
    int index = dropped->voice;
    SNDVOICEVIEW* voices = *(SNDVOICEVIEW**)(sndgs + 468);
    int master = voices[index].master;
    SNDVOICEVIEW* sound;
    int i;

    if (master == -1)
        master = index;
    sound = &voices[master];
    for (i = 0; i < sound->count; i++) {
        if (snddrv.voices[sound->channels[i]].axvoice) {
            if (sound->channels[i] == dropped->voice)
                SNDDRV_freeaxvoice(sound->channels[i], 1);
            else
                SNDDRV_freeaxvoice(sound->channels[i], 2);
            snddrv.voices[sound->channels[i]].allocated = 0;
        }
    }
}
