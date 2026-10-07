// A fragment of sst.c (0x8015bd74): queuing a request on a stream (a file or
// a memory block given to the stream layer, or an external stream handle,
// taken from the stream's free request list with a new serial number in the
// upper bits of its handle), and the public SNDSTRM_create, SNDSTRM_destroy,
// SNDSTRM_queuefile and SNDSTRM_purge. Stream lookup goes through an inline
// range check of the stream index. SNDSTREAMCHANNEL, SNDPLAYOPTS,
// SNDLINKNODE, SNDLINKLIST, SNDSAMPLEFORMAT and SNDSAMPLEATTR are named by the
// mangled symbols; their members and sndgs/sndss are inferred views (see
// sst.c). SNDSTRMI_create, before this unit, is drafted in
// scratch/path/sst_create_wip.cpp.
extern "C" char sndgs[];
extern "C" void* sndss[];

struct SNDPLAYOPTS;
struct TAGGEDPATCH;
struct STREAMCHUNKHDR;

/* inferred members */
struct SNDSAMPLEFORMAT {
    unsigned short rate;
    unsigned char channels;
    unsigned char type;
};

/* inferred members */
struct SNDSAMPLEATTR {
    short unknown00;
    unsigned char unknown02;
    unsigned char unknown03;
    unsigned char unknown04;
    unsigned char unknown05;
    unsigned char unknown06;
    unsigned char unknown07;
    unsigned short unknown08;
    unsigned short unknown0a;
    int unknown0c[2];
    void* buffer[4];
    int size[4];
    unsigned int event[4];
    int eventData[4];
};

/* inferred: a queued stream request (the frame count left and the hold time
   are volatile, as in sst.c) */
struct SNDLINKNODE {
    SNDLINKNODE* next;
    unsigned char unknown04[4];
    int request;
    int handle;
    unsigned int rate;
    unsigned int done;
    unsigned int total;
    volatile unsigned int left;
    volatile int hold;
    unsigned char started;
};

/* inferred: a list of request nodes */
struct SNDLINKLIST {
    SNDLINKNODE* head;
    SNDLINKNODE* tail;
    int count;
};

/* the stream record: named by the mangled symbols, members inferred */
struct SNDSTREAMCHANNEL {
    int handle;
    int voice;
    int player;
    volatile int serial;
    signed char state;
    signed char external;
    unsigned char unknown012[2];
    SNDSAMPLEFORMAT format;
    SNDSAMPLEFORMAT newFormat;
    SNDSAMPLEATTR attr;
    SNDSAMPLEATTR newAttr;
    unsigned char packet[24];
    unsigned char filter[16];
    void* filterSet;
    unsigned char unknown0f0[4];
    SNDLINKLIST active;
    SNDLINKLIST finished;
    SNDLINKNODE* current;
};

void SNDLINKI_init(SNDLINKLIST*);
SNDLINKNODE* SNDLINKI_pop(SNDLINKLIST*);
void SNDLINKI_push(SNDLINKLIST*, SNDLINKNODE*);
void SNDLINKI_pushtail(SNDLINKLIST*, SNDLINKNODE*);
void SNDMEMI_free(void*);
void iSNDserverremoveclient(void (*)(void));
void SNDSTRMI_service(void);
int SNDSTRMI_create(SNDPLAYOPTS*, int, int, void*, int, int, int);

extern "C" {
void* memset(void*, int, unsigned long);
void SNDSYS_entercritical(void);
void SNDSYS_leavecritical(void);
int STREAM_queuefile(int, const char*, int, int);
int STREAM_queuemem(int, void*, int, int);
void STREAM_destroy(int);
void STREAM_kill(int);
void SNDPKTPLAY_stop(int);
void SNDPKTPLAY_destroy(int);
int SNDSTRM_purge(int);
}

/* inferred: the stream an index names, or null */
static inline SNDSTREAMCHANNEL* streamptr(int index) {
    if (index >= *(unsigned char*)(sndgs + 71) || index < 0)
        return 0;
    return (SNDSTREAMCHANNEL*)sndss[index];
}

/* inferred: the number of streams in use */
static inline int streamcount(void) {
    int count = 0;
    int i;

    for (i = 0; i < *(unsigned char*)(sndgs + 71); i++) {
        if (sndss[i])
            count++;
    }
    return count;
}

int SNDSTRMI_queue(int index, int user, char* data, int size, int type) {
    SNDSTREAMCHANNEL* stream = streamptr(index);
    SNDLINKNODE* node;
    int handle;
    int result;

    if (!stream)
        return -8;
    if (!stream->finished.count)
        return -13;
    if (type == 0)
        handle = STREAM_queuefile(stream->handle, data, size, 0x6C454353);
    else if (type == 1)
        handle = STREAM_queuemem(stream->handle, data, 0, 0x6C454353);
    else
        handle = size;
    if (!handle)
        return -1;
    SNDSYS_entercritical();
    node = SNDLINKI_pop(&stream->finished);
    memset(node, 0, sizeof(SNDLINKNODE));
    SNDLINKI_pushtail(&stream->active, node);
    node->request = handle;
    stream->serial += 256;
    if (stream->serial < 0)
        stream->serial = 0;
    node->handle = stream->serial | index;
    node->hold = user;
    result = node->handle;
    SNDSYS_leavecritical();
    return result;
}

extern "C" int SNDSTRM_create(SNDPLAYOPTS* opts, int requests, int players, void* memory, int size) {
    return SNDSTRMI_create(opts, requests, players, memory, size, 0, 0);
}

extern "C" int SNDSTRM_destroy(int index) {
    SNDSTREAMCHANNEL* stream = streamptr(index);

    if (!stream)
        return -8;
    SNDSTRM_purge(index);
    if (streamcount() == 1) {
        iSNDserverremoveclient(SNDSTRMI_service);
        *(void**)(sndgs + 464) = 0;
    }
    SNDPKTPLAY_destroy(stream->player);
    {
        int handle = stream->handle;
        int external = stream->external;

        sndss[index] = 0;
        if (!external)
            STREAM_destroy(handle);
    }
    return 0;
}

extern "C" int SNDSTRM_queuefile(int index, int user, char* name, int size) {
    return SNDSTRMI_queue(index, user, name, size, 0);
}

extern "C" int SNDSTRM_purge(int index) {
    SNDSTREAMCHANNEL* stream;
    SNDLINKNODE* node;
    int i;

    SNDSYS_entercritical();
    stream = streamptr(index);
    if (!stream) {
        SNDSYS_leavecritical();
        return -8;
    }
    if (stream->voice >= 0)
        SNDPKTPLAY_stop(stream->player);
    stream->voice = -1;
    if (!stream->external)
        STREAM_kill(stream->handle);
    if (stream->newAttr.buffer[0]) {
        for (i = 0; i < stream->newFormat.channels; i++)
            SNDMEMI_free(stream->newAttr.buffer[i]);
    }
    do {
        node = SNDLINKI_pop(&stream->active);
        if (node)
            SNDLINKI_push(&stream->finished, node);
    } while (node);
    stream->current = 0;
    stream->state = 0;
    memset(&stream->format, 0, sizeof(SNDSAMPLEFORMAT));
    memset(&stream->newFormat, 0, sizeof(SNDSAMPLEFORMAT));
    memset(&stream->attr, 0, sizeof(SNDSAMPLEATTR));
    memset(&stream->newAttr, 0, sizeof(SNDSAMPLEATTR));
    SNDSYS_leavecritical();
    return 0;
}
