/* A fragment of the sound driver (snddrv.c, 0x8016bb50): destroying a
   packet player's platform record (freeing its two DSP buffers and
   unregistering it) and the fixed output latency. snddrv is named by its
   symbol; its view and the platform record are inferred. */
void SNDPLATFORM_memfree(int, unsigned int);

/* inferred: the packet player's platform record */
struct SNDDRVPACKETVIEW {
    unsigned char buffer[4800];
    unsigned char* staging[3];
    signed char state;
    unsigned char unknown12cd[3];
    void* dsp[2];
    unsigned char unknown12d8[36];
};

struct SNDDRVVIEW {
    unsigned char unknown0000[3716];
    SNDDRVPACKETVIEW* packets[1];
};

extern SNDDRVVIEW snddrv;

int SNDPLATFORM_packetplaydestroy(int index) {
    SNDDRVPACKETVIEW* packet = snddrv.packets[index];

    SNDPLATFORM_memfree(0, (unsigned int)packet->dsp[0]);
    SNDPLATFORM_memfree(0, (unsigned int)packet->dsp[1]);
    snddrv.packets[index] = 0;
    return 0;
}

int SNDPLATFORM_outputlatency(void) {
    return -15;
}
