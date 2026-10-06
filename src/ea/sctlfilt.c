// SNDCTRL_filteradd: add a filter on every platform voice of a sound; returns the voice index (or
// the negative error from the lookup).
struct SNDFILTERDEF;
int SNDVOICEI_get(int);
int iSNDpatchkey(int, int*);
void SNDPLATFORM_filteradd(int, SNDFILTERDEF*);

extern "C" int SNDCTRL_filteradd(int handle, SNDFILTERDEF* filter) {
    int voice = SNDVOICEI_get(handle);

    if (voice >= 0) {
        int key = -1;

        while (iSNDpatchkey(voice, &key))
            SNDPLATFORM_filteradd(key, filter);
    }
    return voice;
}
