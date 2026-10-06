// Fragment of the voice allocator: SNDVOICEI_get maps a sound handle to its
// voice index (the handle's low byte) and checks that the voice is active and
// still owns the handle. SNDVOICEI_alloc and SNDVOICEI_free before it are not
// reconstructed. sndgs's voice table is accessed through inferred offsets.
extern "C" char sndgs[];

int SNDVOICEI_get(int handle) {
    int voice;
    char* entry;

    if (handle < 0)
        return -8;
    voice = handle & 0xff;
    if (voice >= *(short*)(sndgs + 368))
        return -8;
    entry = *(char**)(sndgs + 468) + voice * 128;
    if (!*(signed char*)(entry + 93) || *(int*)entry != handle)
        voice = -8;
    return voice;
}
