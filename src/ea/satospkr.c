/* SNDI_aztospkrvol, the last function of the azimuth-to-speaker file: it
   looks up the precalculated per-speaker levels for an azimuth (its high
   byte selects one of 256 rows) and scales each into a 16-bit volume for
   every output speaker. sndaztospkr and sndgs are named by their symbols;
   the table's row layout and the view of sndgs (the speaker count at +72)
   are inferred. */
struct SNDGSVIEW {
    unsigned char unknown000[72];
    unsigned char speakers;
    unsigned char unknown049[495];
};

extern signed char sndaztospkr[256][4];
extern SNDGSVIEW sndgs;

void SNDI_aztospkrvol(int azimuth, short* volumes) {
    signed char* levels = sndaztospkr[(azimuth >> 8) & 0xFF];
    int i = 0;
    while (i < sndgs.speakers) {
        int level = *levels++;
        i++;
        *volumes++ = level * 258;
    }
}
