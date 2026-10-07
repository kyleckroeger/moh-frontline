/* A fragment of the sound driver (snddrv.c, 0x8016c5f8): filter
   installation (unsupported) and memory allocation and freeing through the
   installed allocator or ARAM (sizes rounded up to 32 bytes). sndgs is named
   by its symbol; its view is inferred. SNDPLATFORM_download and
   SNDPLATFORM_downloadcomplete after it (ARQ transfers through an inline
   that posts a request in a new or given DMA slot) are drafted in
   scratch/path/sdrv_tail4.cpp and scratch/path/sdrv_dlc_wip.cpp. */
struct SNDFILTERDEF;

extern "C" char sndgs[];
void* SNDARAM_alloc(int);
void SNDARAM_free(unsigned int);

int SNDPLATFORM_filteradd(int, SNDFILTERDEF*) {
    return -15;
}

void* SNDPLATFORM_memalloc(int, int size) {
    size = (size + 31) & ~31;
    if (*(void* (**)(int))(sndgs + 184))
        return (*(void* (**)(int))(sndgs + 184))(size);
    return SNDARAM_alloc(size);
}

int SNDPLATFORM_memfree(int, unsigned int memory) {
    if (*(void (**)(unsigned int))(sndgs + 188)) {
        (*(void (**)(unsigned int))(sndgs + 188))(memory);
        return 0;
    }
    SNDARAM_free(memory);
    return 0;
}

