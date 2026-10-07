/* The sound driver's mixing thread and the platform output capabilities (a
   fragment of snddrv.c). The thread runs the sndgs hooks, mixes a 160-sample
   slice into the mix buffers and runs the 100 Hz server every other frame;
   SNDPLATFORM_outputcaps fills sndgs with the GameCube output parameters and
   the ARAM range. sndgs and snddrv are named by their symbols; the views of
   them are inferred from offsets. everyother is the file's frame counter
   (static in the original; declared here as an external). */
struct _AXVPB;
struct OSThreadQueue;

extern "C" {
extern char sndgs[];
void OSSleepThread(OSThreadQueue*);
unsigned long ARGetBaseAddress(void);
extern void* MIXaudioslicefn;
}
void SNDPKTPLAYI_flushcallbackdata(void);
void SNDSYSI_100hzserver(void);
void SNDI_mutexlock(void);
void SNDI_mutexunlock(void);

/* inferred view of snddrv: 61 per-channel records, then the DMA request
   table, the mixer flags and the mix buffers */
struct SNDDRVVOICE {
    char field00[8];
    int field08;
    char field0C[3];
    unsigned char active;
    char field10[4];
    unsigned char field14;
    unsigned char field15;
    char field16[22];
    _AXVPB* axvoice;
};

struct SNDDRVDMA {
    int id;
    char field04[60];
};

struct OSThreadQueue {
    void* head;
    void* tail;
};

struct SNDDRVVIEW {
    SNDDRVVOICE voices[61];
    char fieldb70[0x1858 - 61 * 48];
    OSThreadQueue threadqueue;
    char field1860[0x9860 - 0x1860];
    int lastdma;
    char field9864[0x98a8 - 0x9864];
    SNDDRVDMA dma[64];
    char fielda8a8[0xaac0 - 0xa8a8];
    signed char mixerinited;
    unsigned char activevoices;
    char fieldaac2[0xaac8 - 0xaac2];
    void* mixalloc[4];
    void* mixbuffer[3];
    char fieldaae4[0xab20 - 0xaae4];
};

extern SNDDRVVIEW snddrv;

extern int everyother;

void SNDDRV_thread(void* param) {
    for (;;) {
        SNDI_mutexlock();
        if (*(void (**)(void))(sndgs + 552))
            (*(void (**)(void))(sndgs + 552))();
        if (snddrv.mixerinited) {
            ((void (*)(void**, int))MIXaudioslicefn)(snddrv.mixbuffer, 160);
            SNDPKTPLAYI_flushcallbackdata();
        }
        if (everyother & 1)
            SNDSYSI_100hzserver();
        if (*(void (**)(void))(sndgs + 556))
            (*(void (**)(void))(sndgs + 556))();
        SNDI_mutexunlock();
        OSSleepThread(&snddrv.threadqueue);
    }
}

/* sndgs fields by offset */
#define U8(o) (*(unsigned char*)(sndgs + (o)))
#define U16(o) (*(unsigned short*)(sndgs + (o)))
#define U32(o) (*(unsigned long*)(sndgs + (o)))

int SNDPLATFORM_outputcaps() {
    unsigned long base;

    U8(6) = 16;
    U8(11) = 61;
    U16(0) = 32000;
    U16(2) = 32000;
    U8(4) = 1;
    U8(5) = 3;
    U8(74) = 1;
    U8(19) = 2;
    U16(20) = 544;
    U16(22) = 36;
    U8(77) = 2;
    U16(80) = 544;
    U16(82) = 36;
    U8(46) = 8;
    U8(51) = 48;
    U8(71) = 8;
    U16(42) = 32000;
    U8(72) = 2;
    U8(70) = 1;
    base = ARGetBaseAddress() + 256;
    U32(92) = base;
    U32(96) = 0x800000 - base;
    U8(52) = 10;
    U16(120) = 0;
    U16(128) = 0x4000;
    U16(130) = 0xc000;
    U16(136) = 0x4000;
    U16(138) = 0x8000;
    U16(140) = 0xc000;
    return 0;
}
