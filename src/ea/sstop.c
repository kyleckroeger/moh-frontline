// SNDstop: stop every platform voice of a sound; returns the voice index (or
// the negative error from the lookup).
int SNDVOICEI_get(int);
int iSNDpatchkey(int, int*);
void SNDPLATFORM_stop(int);

extern "C" int SNDstop(int handle) {
    int voice = SNDVOICEI_get(handle);

    if (voice >= 0) {
        int key = -1;

        while (iSNDpatchkey(voice, &key))
            SNDPLATFORM_stop(key);
    }
    return voice;
}
