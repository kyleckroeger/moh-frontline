/* A fragment of the sound driver (snddrv.c, 0x8016bdb4): the dry level,
   applied through the volume for AX and mixer voices. sndgs is named by its
   symbol; its view is inferred. The voice record's flags are read through a
   byte offset into the voice table (the original adds the field offset to
   the scaled index before the table base). */
extern "C" char sndgs[];

void SNDPLATFORM_setvol(int);

void SNDPLATFORM_setdrylevel(int voice) {
    if (*(unsigned short*)(*(char**)(sndgs + 468) + voice * 128 + 32) & 0x200 || *(unsigned short*)(*(char**)(sndgs + 468) + voice * 128 + 32) & 4)
        SNDPLATFORM_setvol(voice);
}
