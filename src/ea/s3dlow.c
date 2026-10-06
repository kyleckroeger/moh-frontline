// SND3dpos: set a sound's 3D position on every platform voice (azimuth
// relative to the voice's base azimuth, distance clamped to 15 bits). sndgs's
// layout is not known; the voice table is accessed through inferred offsets.
extern "C" char sndgs[];
int SNDVOICEI_get(int);
int iSNDpatchkey(int, int*);
void SNDPLATFORM_set3dpos(int);

extern "C" int SND3dpos(int handle, int azimuth, int distance) {
    int voice;

    if (distance > 16383)
        distance = 16383;
    else if (distance < -16384)
        distance = -16384;
    voice = SNDVOICEI_get(handle);
    if (voice >= 0) {
        int key = -1;

        while (iSNDpatchkey(voice, &key)) {
            char* entry = *(char**)(sndgs + 468) + key * 128;

            *(short*)(entry + 24) = *(unsigned short*)(entry + 70) + azimuth;
            *(short*)(entry + 26) = distance;
            SNDPLATFORM_set3dpos(key);
        }
    }
    return voice;
}
