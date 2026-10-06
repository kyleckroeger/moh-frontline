// SNDCTRL_highpass: set the high-pass cutoff on every platform voice of a sound; returns the voice index (or
// the negative error from the lookup).
int SNDVOICEI_get(int);
int iSNDpatchkey(int, int*);
void SNDPLATFORM_highpass(int, int);

extern "C" int SNDCTRL_highpass(int handle, int frequency) {
    int voice = SNDVOICEI_get(handle);

    if (voice >= 0) {
        int key = -1;

        while (iSNDpatchkey(voice, &key))
            SNDPLATFORM_highpass(key, frequency);
    }
    return voice;
}
