// A fragment of EA's stream.cpp (0x8014cde0): STREAM_setgreedylevel (stores
// the level and switches the greedy state when the buffered count's side of
// the level changes), STREAM_setgreedystate (stores the greedy state and,
// when it is set while the stream is reading, raises the file operation's
// priority), STREAM_taphandle (the 16-byte tap entry for a 1-based tap
// number, or null), and STREAM_queuefile and STREAM_queuemem, which take a
// request from the free list (giving it a new id in the upper bits of its
// index), fill it in (a file name, or a memory block whose size is found by
// walking its chunks to the end chunk when it is not given), append it to
// the queue, and start an idle stream; they return the request handle. The
// file name is this project's; the original record is stream.cpp
// (stream_cb.cpp, stream_prio.cpp and stream.cpp hold other parts of it).
// The handle check is the same inferred inline as in stream.cpp;
// STREAMHEADERtag is named by the mangled symbols, the request counter by
// its local symbol, and the header, request and handle layouts and the
// helpers are inferred. STREAM_cancelrequest, next in the file, is drafted
// in scratch/game/stream_cancel_wip.cpp (one register differs).
// EA's streaming layer. Every public function validates the handle through
// the same inline check (non-null handle whose header carries the stream
// magic). STREAMHEADERtag is named by the mangled symbols; the header and
// handle layouts, the magic's meaning and the inline helper are inferred.
/* inferred: a stream request (REQUESTSTRUCTtag in the mangled names) */
struct REQUESTVIEW {
    int index;
    int state;
    REQUESTVIEW* prev;
    REQUESTVIEW* next;
    int type;
    char filename[256];
    unsigned char* source;
    int size;
    int id;
    unsigned char* start;
};

struct STREAMHEADERtag {
    int magic;
    unsigned char mutex[28];
    REQUESTVIEW* requests;
    int requestCount;
    unsigned char unknown28[8];
    unsigned char* taps;
    int tapCount;
    int bufferStart;
    unsigned char* chunkStart;
    int bufferEnd;
    int state;
    int priority;
    int priorityClass;
    int greedyLevel;
    int greedyState;
    int buffered;
    unsigned char* read;
    unsigned char* requestStart;
    unsigned char unknown64[4];
    REQUESTVIEW* first;
    REQUESTVIEW* current;
    REQUESTVIEW* last;
    REQUESTVIEW* free;
    unsigned char unknown78[264];
    int fileOp;
};

struct STREAMHANDLEVIEW {
    STREAMHEADERtag* header;
    int number;
    int size;
    unsigned char* first;
};

extern "C" {
int FILESYS_priorityop(int, int);
void MUTEX_lock(void*);
void MUTEX_unlock(void*);
char* strncpy(char*, const char*, int);
}

// File-local in the original (defined elsewhere in the file).
void startnextrequest(STREAMHEADERtag*, int);
extern int requestidcounter;

#define STREAM_GET32(p) (((p)[3] << 24) | ((p)[2] << 16) | ((p)[1] << 8) | (p)[0])

static inline int validhandle(int handle, STREAMHANDLEVIEW** out, STREAMHEADERtag** header) {
    int error;
    if (handle == 0) {
        error = 1;
    } else if (((STREAMHANDLEVIEW*)handle)->header->magic != 0x4D525453) {
        error = 1;
    } else {
        *out = (STREAMHANDLEVIEW*)handle;
        *header = ((STREAMHANDLEVIEW*)handle)->header;
        error = 0;
    }
    return error;
}

extern "C" void STREAM_setgreedystate(int handle, int state);

extern "C" void STREAM_setpriority(int handle, int priority, int priorityClass);

extern "C" void STREAM_setgreedylevel(int handle, int level) {
    STREAMHANDLEVIEW* stream;
    STREAMHEADERtag* header;
    if (validhandle(handle, &stream, &header) == 0) {
        int old = header->greedyLevel;
        int now;
        int was;
        header->greedyLevel = level;
        now = header->buffered < level;
        was = header->buffered < old;
        if (was != now)
            STREAM_setgreedystate(handle, now);
    }
}

extern "C" void STREAM_setgreedystate(int handle, int state) {
    STREAMHANDLEVIEW* stream;
    STREAMHEADERtag* header;
    if (validhandle(handle, &stream, &header) == 0) {
        header->greedyState = state;
        if (state && header->state == 1)
            FILESYS_priorityop(header->fileOp, header->priorityClass);
    }
}

extern "C" void* STREAM_taphandle(int handle, int tap) {
    STREAMHANDLEVIEW* stream;
    STREAMHEADERtag* header;
    if (validhandle(handle, &stream, &header) != 0)
        return 0;
    if (tap < 1 || tap > header->tapCount)
        return 0;
    return header->taps + (tap - 1) * 16;
}

extern "C" void* STREAM_gettable(int handle);

extern "C" int STREAM_state(int handle);

extern "C" int STREAM_isendofstream(int handle);

extern "C" int STREAM_buffersize(int handle);

/* inferred: takes a request from the free list and gives it a new id in the
   upper bits of its index */
static inline REQUESTVIEW* allocrequest(STREAMHEADERtag* header) {
    REQUESTVIEW* request;

    MUTEX_lock(header->mutex);
    if (header->free == 0) {
        request = 0;
    } else {
        request = header->free;
        header->free = request->next;
        if ((requestidcounter += 256) == 0)
            requestidcounter = 256;
        request->index = (request->index & 0xFF) | requestidcounter;
    }
    MUTEX_unlock(header->mutex);
    return request;
}

/* inferred: appends a request to the queue */
static inline void addrequest(STREAMHEADERtag* header, REQUESTVIEW* request) {
    MUTEX_lock(header->mutex);
    if (header->last == 0) {
        request->prev = 0;
        header->first = request;
        header->current = request;
        header->last = request;
    } else {
        request->prev = header->last;
        header->last->next = request;
        header->last = request;
    }
    MUTEX_unlock(header->mutex);
}

/* inferred: marks an idle stream as reading; true when it was idle */
static inline int startstream(STREAMHEADERtag* header) {
    int state;

    MUTEX_lock(header->mutex);
    state = header->state;
    if (state == 0)
        header->state = 1;
    MUTEX_unlock(header->mutex);
    return state == 0;
}

extern "C" int STREAM_queuefile(int handle, const char* name, int size, int id) {
    STREAMHANDLEVIEW* stream;
    STREAMHEADERtag* header;
    REQUESTVIEW* request;

    if (validhandle(handle, &stream, &header) != 0)
        return 0;
    request = allocrequest(header);
    if (request == 0)
        return 0;
    request->type = 0;
    strncpy(request->filename, name, 254);
    request->size = size;
    request->id = id;
    request->state = 1;
    request->next = 0;
    addrequest(header, request);
    if (startstream(header)) {
        if (header->greedyState)
            startnextrequest(header, header->priorityClass);
        else
            startnextrequest(header, header->priority);
    }
    return request->index;
}

extern "C" int STREAM_queuemem(int handle, unsigned char* data, int size, int id) {
    STREAMHANDLEVIEW* stream;
    STREAMHEADERtag* header;
    REQUESTVIEW* request;

    if (validhandle(handle, &stream, &header) != 0)
        return 0;
    request = allocrequest(header);
    if (request == 0)
        return 0;
    if (size == 0) {
        unsigned char* chunk = data;

        while (STREAM_GET32(chunk) != id) {
            size += STREAM_GET32(chunk + 4);
            chunk += STREAM_GET32(chunk + 4);
        }
        size += STREAM_GET32(chunk + 4);
    }
    request->type = 1;
    request->source = data;
    request->size = size;
    request->id = id;
    request->state = 1;
    request->next = 0;
    addrequest(header, request);
    if (startstream(header))
        startnextrequest(header, 0);
    return request->index;
}
