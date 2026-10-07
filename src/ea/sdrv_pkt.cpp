/* A fragment of the sound driver (snddrv.c, 0x8016b6b0): the packet
   player's platform overhead and creation (two 3 KB DSP buffers and three
   32-byte aligned staging areas in the caller's memory). snddrv is named by
   its symbol; its view and the packet player's platform record are
   inferred. SNDDRV_getmastervoice and SNDDRV_getsamplechan before it are not
   reconstructed (index addressing and a compare operand order differ). */
extern "C" {
void* memset(void*, int, unsigned long);
}
void* SNDPLATFORM_memalloc(int, int);

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

int SNDPLATFORM_packetoverhead(void) {
    return sizeof(SNDDRVPACKETVIEW);
}

int SNDPLATFORM_packetplaycreate(int index, void* memory) {
    SNDDRVPACKETVIEW* packet = (SNDDRVPACKETVIEW*)memory;

    memset(packet, 0, sizeof(SNDDRVPACKETVIEW));
    packet->dsp[0] = SNDPLATFORM_memalloc(0, 3072);
    if (!packet->dsp[0])
        return -9;
    packet->dsp[1] = SNDPLATFORM_memalloc(0, 3072);
    if (!packet->dsp[1])
        return -9;
    packet->staging[0] = (unsigned char*)(((unsigned int)packet->buffer + 31) & ~31);
    packet->staging[1] = (unsigned char*)(((unsigned int)packet->buffer + 1631) & ~31);
    packet->staging[2] = (unsigned char*)(((unsigned int)packet->buffer + 3231) & ~31);
    packet->state = -1;
    snddrv.packets[index] = packet;
    return 0;
}
