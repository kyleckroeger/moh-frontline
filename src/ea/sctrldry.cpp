// SNDCTRL_drylevel: set the dry level on every platform voice of a sound;
// returns the voice index (or the negative error from the lookup). sndgs's
// layout is not known; the voice table is accessed through inferred offsets.
extern "C" char sndgs[];
int SNDVOICEI_get(int);
int iSNDpatchkey(int, int*);
void SNDPLATFORM_setdrylevel(int);

extern "C" int SNDCTRL_drylevel(int handle, int level) {
    int voice = SNDVOICEI_get(handle);

    if (voice >= 0) {
        int key = -1;

        while (iSNDpatchkey(voice, &key)) {
            *(char*)(*(char**)(sndgs + 468) + key * 128 + 83) = level;
            SNDPLATFORM_setdrylevel(key);
        }
    }
    return voice;
}
