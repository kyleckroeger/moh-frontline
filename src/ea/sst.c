// Sound streams: stream lookup and teardown, the chunk release callback, the
// frames callback that advances the current request (finishing it, or after
// 200 passes, onto the stream's finished list) and carries surplus frames to
// the next one, and the header parser, which moves to the next request,
// converts the chunk's patch into the pending sample format and attributes,
// reports the attributes' events to the registered callbacks, releases the
// chunk, and then adopts the new format (stalling, state 2, when a playing
// stream's format changes) and starts the packet player. SNDSAMPLEFORMAT,
// SNDSAMPLEATTR, SNDSAMPLEDESC, SNDLINKNODE, SNDLINKLIST, STREAMCHUNKHDR and
// TAGGEDPATCH are named by the mangled symbols; their members, the stream
// records and sndgs/sndss are inferred views.
extern "C" char sndgs[];
extern "C" void* sndss[];

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

/* inferred: filled in by the patch conversion, starting with the sample's
   frame count */
struct SNDSAMPLEDESC {
    unsigned int frames;
    unsigned char unknown04[16];
};

void SNDI_patchtohdr(void*, TAGGEDPATCH*, SNDSAMPLEFORMAT*, SNDSAMPLEATTR*, SNDSAMPLEDESC*);

extern "C" {
int SNDSTRM_destroy(int);
void STREAM_release(int, unsigned char*);
int memcmp(const void*, const void*, unsigned long);
int SNDPKTPLAY_start(int, SNDSAMPLEFORMAT*, SNDSAMPLEATTR*, void*);
void SNDCTRL_filteradd(int, void*);
}

// SNDSTRMI_calcdatarate comes first in the original file; it is drafted in
// scratch but differs in register numbering, so this unit starts after it.

int SNDSTRMI_destroyall(void) {
    for (int i = 0; i < *(unsigned char*)(sndgs + 71); i++)
        SNDSTRM_destroy(i);
    return 0;
}

void* SNDSTRMI_getstreamptr(int handle) {
    if (handle >= *(unsigned char*)(sndgs + 71) || handle < 0)
        return 0;
    return sndss[handle];
}

void SNDSTRMI_releasecallback(void* data, void*) {
    unsigned char* chunk = *(unsigned char**)((char*)data - 4);

    STREAM_release(*(int*)sndss[(unsigned char)*(int*)chunk], chunk);
}

/* inferred: a queued stream request, linked by its first fields (the frame
   count left is volatile: the callback rereads it after the comparison) */
struct SNDLINKNODE {
    SNDLINKNODE* next;
    unsigned char unknown04[8];
    int handle;
    int rate;
    unsigned int done;
    unsigned int total;
    volatile unsigned int left;
    unsigned char unknown20[4];
    unsigned char started;
};

/* inferred: a list of request nodes */
struct SNDLINKLIST {
    SNDLINKNODE* head;
    unsigned char unknown04[8];
};

/* inferred: the stream record's request lists */
struct SNDSTREAMVIEW {
    int handle;
    int voice;
    int player;
    unsigned char unknown00c[4];
    signed char state;
    unsigned char unknown011[3];
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

SNDLINKNODE* SNDSTRMI_getrequestptr(int);
void SNDLINKI_remove(SNDLINKLIST*, SNDLINKNODE*);
void SNDLINKI_push(SNDLINKLIST*, SNDLINKNODE*);

/* inferred: moves a request to the stream's finished list */
static inline void finishrequest(SNDLINKNODE* request) {
    SNDSTREAMVIEW* stream = (SNDSTREAMVIEW*)sndss[request->handle & 0xFF];
    SNDLINKNODE* node = SNDSTRMI_getrequestptr(request->handle);

    SNDLINKI_remove(&stream->active, node);
    SNDLINKI_push(&stream->finished, node);
    if (stream->current == node)
        stream->current = 0;
}

void SNDSTRMI_framescallback(int, int frames, void* data) {
    SNDSTREAMVIEW* stream = (SNDSTREAMVIEW*)data;
    int extra = 0;
    int count = 0;

    for (;;) {
        SNDLINKNODE* request = stream->active.head;

        if (++count > 200) {
            finishrequest(request);
            return;
        }
        if (frames > request->left) {
            extra = frames - request->left;
            frames -= extra;
        }
        request->done += frames;
        request->left -= frames;
        if (request->done >= request->total)
            finishrequest(request);
        if (!extra)
            return;
        frames = extra;
        extra = 0;
    }
}

/* inferred: the notification passed to the registered callbacks */
struct SNDSTREAMEVENT {
    int type;
    int event;
    int data;
    int unknown0c;
    int handle;
};

int SNDSTRMI_calcdatarate(SNDSAMPLEFORMAT*);

int SNDSTRMI_parseheader(int index, STREAMCHUNKHDR* chunk) {
    SNDSTREAMVIEW* stream = (SNDSTREAMVIEW*)sndss[index];
    SNDLINKNODE* request;
    SNDSAMPLEDESC desc;
    SNDSTREAMEVENT event;
    int i;
    int j;

    if (!stream->current)
        stream->current = stream->active.head;
    else
        stream->current = stream->current->next;
    request = stream->current;
    SNDI_patchtohdr(0, (TAGGEDPATCH*)((char*)chunk + 8), &stream->newFormat, &stream->newAttr, &desc);
    request->total = desc.frames;
    request->started = 0;
    i = 0;
    while (stream->newAttr.event[i]) {
        event.type = 3;
        event.event = stream->newAttr.event[i];
        event.data = stream->newAttr.eventData[i];
        event.handle = request->handle;
        stream->newAttr.event[i] = 0;
        stream->newAttr.eventData[i] = 0;
        i++;
        for (j = 0; j < *(signed char*)(sndgs + 366); j++)
            (*(void (**)(SNDSTREAMEVENT*))(sndgs + 424 + j * 4))(&event);
    }
    STREAM_release(stream->handle, (unsigned char*)chunk);
    request->rate = SNDSTRMI_calcdatarate(&stream->newFormat);
    if (memcmp(&stream->format, &stream->newFormat, sizeof(SNDSAMPLEFORMAT)) != 0
        || memcmp(&stream->attr, &stream->newAttr, sizeof(SNDSAMPLEATTR)) != 0 || stream->newAttr.buffer[0]) {
        if (stream->format.rate) {
            stream->state = 2;
            return 0;
        }
        stream->format = stream->newFormat;
        stream->attr = stream->newAttr;
        stream->newAttr.buffer[0] = 0;
    }
    if (stream->state != 1) {
        stream->voice = SNDPKTPLAY_start(stream->player, &stream->format, &stream->attr, stream->packet);
        if (stream->filterSet)
            SNDCTRL_filteradd(stream->voice, stream->filter);
        stream->state = 1;
        stream->state = 1;
    }
    return 0;
}

// SNDSTRMI_parsedata and the stream functions after it are not
// reconstructed.
