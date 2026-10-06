/* SNDvol: sets the volume of every channel playing a voice's patch, skipping
   the work when a channel already has that volume. sndgs and the functions
   are named by their symbols; the views of sndgs (the channel table) and of
   a 128-byte channel are inferred. */
struct SNDCHANVIEW {
    unsigned char unknown00[4];
    short fxSend;
    unsigned char unknown06[38];
    int dirty;
    unsigned char unknown30[4];
    int volume;
    unsigned char unknown38[30];
    short fxLevel;
    unsigned char unknown58[40];
};

struct SNDGSVIEW {
    unsigned char unknown000[468];
    SNDCHANVIEW* chans;
    unsigned char unknown1d8[96];
};

extern SNDGSVIEW sndgs;

int SNDVOICEI_get(int);
int iSNDpatchkey(int, int*);
void iSNDcalcvol(int);
void SNDPLATFORM_setvol(int);
void SNDPLATFORM_setfxlevel(int, int);

extern "C" int SNDvol(int handle, int volume) {
    SNDCHANVIEW* chan;
    int key;
    int voice = SNDVOICEI_get(handle);
    if (voice >= 0) {
        key = -1;
        while (iSNDpatchkey(voice, &key)) {
            chan = &sndgs.chans[key];
            chan->dirty = 0;
            if (chan->volume == volume << 16)
                return 0;
            chan->volume = volume << 16;
            iSNDcalcvol(key);
            SNDPLATFORM_setvol(key);
            if (chan->fxLevel > 0)
                SNDPLATFORM_setfxlevel(chan->fxSend, 0);
        }
    }
    return voice;
}
