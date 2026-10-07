/* A fragment of the sound driver (snddrv.c, 0x8016b600): a driver voice's
   master voice (itself when it has none). sndgs is named by its symbol; its
   view is inferred. The voice record's master field is read through a byte
   offset into the voice table (the original adds the field offset to the
   scaled index before the table base). SNDDRV_getsamplechan after it is not
   reconstructed (a compare operand order differs). */
extern "C" char sndgs[];

int SNDDRV_getmastervoice(int voice) {
    int index = voice + *(unsigned char*)(sndgs + 51);
    int master = *(short*)(*(char**)(sndgs + 468) + index * 128 + 36);

    if (master == -1)
        master = index;
    return master;
}
