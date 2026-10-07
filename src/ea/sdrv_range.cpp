/* A fragment of the sound driver (snddrv.c, 0x8016b0f0): the voice range a
   render mode may use (mode bit 0x200: none below the first driver voice;
   bit 4: from the first driver voice to the voice count) and resolving a
   timbre's sample data asynchronously (the data offset is taken from the
   header when none is given, and the sample pointer becomes the buffer plus
   the remaining offset). sndgs is named by its symbol; its view and the
   patch header's members are inferred. */
extern "C" char sndgs[];

/* inferred members */
struct SNDIPATCHHEADER {
    unsigned char unknown00[100];
    unsigned char** data;
    int offset;
};

void SNDPLATFORM_getvoicerange(int mode, int* low, int* high) {
    if (mode & 0x200) {
        *low = 0;
        *high = *(unsigned char*)(sndgs + 51);
    } else if (mode & 4) {
        *low = *(unsigned char*)(sndgs + 51);
        *high = *(short*)(sndgs + 368);
    }
}

int SNDPLATFORM_asyncresolvetimbre(SNDIPATCHHEADER* header, char* buffer, int* offset) {
    if (*offset == 0)
        *offset = header->offset;
    *header->data = (unsigned char*)(buffer + (header->offset - *offset));
    return 0;
}
