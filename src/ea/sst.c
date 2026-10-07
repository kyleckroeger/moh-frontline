// Sound streams: stream lookup and teardown, and the frames callback that
// advances the current request (finishing it, or after 200 passes, onto the
// stream's finished list) and carries surplus frames to the next one. The
// stream records and sndgs/sndss are inferred views.
extern "C" char sndgs[];
extern "C" void* sndss[];

extern "C" {
int SNDSTRM_destroy(int);
void STREAM_release(void*);
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
    STREAM_release(*(void**)sndss[(unsigned char)**(int**)((char*)data - 4)]);
}

/* inferred: a queued stream request, linked by its first fields (the frame
   count left is volatile: the callback rereads it after the comparison) */
struct SNDLINKNODE {
    unsigned char unknown00[12];
    int handle;
    unsigned char unknown10[4];
    unsigned int done;
    unsigned int total;
    volatile unsigned int left;
};

/* inferred: a list of request nodes */
struct SNDLINKLIST {
    SNDLINKNODE* head;
    unsigned char unknown04[8];
};

/* inferred: the stream record's request lists */
struct SNDSTREAMVIEW {
    unsigned char unknown000[244];
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

// The stream functions after SNDSTRMI_framescallback are not reconstructed.
