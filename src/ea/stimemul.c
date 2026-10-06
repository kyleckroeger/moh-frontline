// SNDCTRL_timemult: set the time multiplier on every platform voice of a
// sound. sndgs's layout is not known; the voice table is accessed through
// inferred offsets.
extern "C" char sndgs[];
int SNDVOICEI_get(int);
int iSNDpatchkey(int, int*);
void SNDPLATFORM_timemult(int, int);

extern "C" int SNDCTRL_timemult(int handle, int multiplier) {
    int voice = SNDVOICEI_get(handle);

    if (voice >= 0) {
        int key = -1;

        while (iSNDpatchkey(voice, &key)) {
            *(short*)(*(char**)(sndgs + 468) + key * 128 + 94) = multiplier;
            SNDPLATFORM_timemult(key, multiplier);
        }
    }
    return 0;
}
