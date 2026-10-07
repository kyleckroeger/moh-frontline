/* A fragment of EA's streaming layer (stream.cpp, 0x8014bee0): the file
   operation callbacks and the buffer refill. opencallback records the
   opened handle and restarts the stream; closecallback reopens the stream's
   file and installs opencallback; readcallback adds the bytes read to the
   file position and the write pointer, parses the new chunks, and then
   starts the next request when this one is finished (its whole file read,
   or a short read, or chunks parsed), marking it done, or restarts the
   stream; startnextrequest moves to the next queued request (or marks the
   stream idle), resets the read position, and switches files (closing the
   open one, or opening the new one) when the request names another file,
   else restarts the stream. restartstream skips consumed chunks (an id of
   -1 wraps to the chunk start, -2 skips its length), frees finished
   requests whose data left the unread part of the ring, finds the free
   space (moving the partial request to the buffer start behind a wrap
   marker when the tail is too short), and then copies the next part of a
   memory request or starts a file read; the stream is stalled (state 2)
   while there is too little room. STREAMHEADERtag and REQUESTSTRUCTtag are
   named by the mangled symbols; their members, the request view and the
   helpers' signatures are inferred. */
/* inferred: a stream request (state, type, file size) */
struct REQUESTVIEW {
    unsigned char unknown00[4];
    int state;
    unsigned char unknown08[4];
    REQUESTVIEW* next;
    int type;
    char filename[256];
    unsigned char* source;
    int size;
    unsigned char unknown11c[4];
    unsigned char* start;
};

struct REQUESTSTRUCTtag;

struct STREAMHEADERtag {
    int magic;
    unsigned char mutex[44];
    unsigned char unknown30[8];
    unsigned char* bufferStart;
    unsigned char* chunkStart;
    unsigned char* bufferEnd;
    int state;
    unsigned char unknown48[4];
    int priority;
    unsigned char unknown50[12];
    unsigned char* read;
    unsigned char* requestStart;
    unsigned char* write;
    REQUESTVIEW* first;
    REQUESTVIEW* current;
    unsigned char unknown70[8];
    char filename[256];
    int handle;
    int position;
    int fileOp;
    int requested;
    int minimum;
};

extern "C" {
int FILESYS_completeop(int);
int FILESYS_open(const char*, int, int, void*);
void FILESYS_callbackop(int, void (*)(int, int, void*));
int FILESYS_close(int, int, void*);
int FILESYS_read(int, int, void*, int, int, void*);
int strcmp(const char*, const char*);
char* strcpy(char*, const char*);
void MUTEX_lock(void*);
void MUTEX_unlock(void*);
void MEM_copy(void*, const void*, int);
}

// File-local in the original (defined elsewhere in the file).
int parsechunks(STREAMHEADERtag*);
void freerequest(STREAMHEADERtag*, REQUESTSTRUCTtag*);
static void restartstream(STREAMHEADERtag*, int);
static void startnextrequest(STREAMHEADERtag*, int);

static void opencallback(int, int, void* data) {
    STREAMHEADERtag* header = (STREAMHEADERtag*)data;

    header->handle = FILESYS_completeop(header->fileOp);
    if (header->handle)
        restartstream(header, header->priority);
}

static void closecallback(int, int, void* data) {
    STREAMHEADERtag* header = (STREAMHEADERtag*)data;

    FILESYS_completeop(header->fileOp);
    header->fileOp = FILESYS_open(header->filename, 1, header->priority, header);
    if (header->fileOp)
        FILESYS_callbackop(header->fileOp, opencallback);
}

static void readcallback(int, int, void* data) {
    STREAMHEADERtag* header = (STREAMHEADERtag*)data;
    REQUESTVIEW* request = header->current;
    int done;
    int count;
    int parsed;

    if (request->type == 1) {
        count = header->requested;
        done = header->position + count >= request->size;
    } else {
        count = FILESYS_completeop(header->fileOp);
        done = count < header->requested;
    }
    header->position += count;
    header->write += count;
    parsed = parsechunks(header);
    if (request->state == 4) {
        startnextrequest(header, header->priority);
    } else if (done || parsed) {
        MUTEX_lock(header->mutex);
        if (request->state != 4)
            request->state = 3;
        MUTEX_unlock(header->mutex);
        startnextrequest(header, header->priority);
    } else {
        restartstream(header, header->priority);
    }
}

static void startnextrequest(STREAMHEADERtag* header, int priority) {
    REQUESTVIEW* request = 0;
    int idle;

    MUTEX_lock(header->mutex);
    if (!header->current)
        idle = 1;
    else if (header->current->state == 1)
        idle = 0;
    else if (!header->current->next)
        idle = 1;
    else {
        header->current = header->current->next;
        idle = 0;
    }
    if (idle) {
        header->state = 0;
    } else {
        request = header->current;
        request->start = header->requestStart;
        request->state = 2;
    }
    MUTEX_unlock(header->mutex);
    if (idle)
        return;
    header->write = header->requestStart;
    if (request->type == 1) {
        header->position = 0;
    } else {
        header->position = request->size;
        if (strcmp(request->filename, header->filename)) {
            strcpy(header->filename, request->filename);
            if (!header->handle) {
                header->fileOp = FILESYS_open(header->filename, 1, priority, header);
                if (header->fileOp)
                    FILESYS_callbackop(header->fileOp, opencallback);
            } else {
                header->fileOp = FILESYS_close(header->handle, priority, header);
                if (header->fileOp)
                    FILESYS_callbackop(header->fileOp, closecallback);
            }
            return;
        }
    }
    restartstream(header, priority);
}

#define STREAM_GET32(p) (((p)[3] << 24) | ((p)[2] << 16) | ((p)[1] << 8) | (p)[0])

static void restartstream(STREAMHEADERtag* header, int priority) {
    int space;
    int length;
    REQUESTVIEW* request;

    while (header->read != header->requestStart) {
        int id = STREAM_GET32(header->read);

        if (id == -1)
            header->read = header->chunkStart;
        else if (id == -2)
            header->read += STREAM_GET32(header->read + 4);
        else
            break;
    }
    MUTEX_lock(header->mutex);
    for (;;) {
        REQUESTVIEW* next = header->first->next;
        int inside;
        unsigned char* read;
        unsigned char* write;
        unsigned char* start;

        if (!next || next->state == 1)
            break;
        write = header->write;
        read = header->read;
        start = next->start;
        if (read <= write)
            inside = start - 1 >= read && start - 1 < write;
        else
            inside = start - 1 >= read || start - 1 < write;
        if (inside)
            break;
        freerequest(header, (REQUESTSTRUCTtag*)header->first);
    }
    MUTEX_unlock(header->mutex);
    if (header->read > header->write) {
        space = header->read - header->write - 33;
    } else {
        space = header->bufferEnd - header->write - 32;
        if (space < header->minimum) {
            int rest;
            unsigned char* marker;

            request = header->current;
            length = header->write - header->requestStart;
            if (request->type == 1) {
                if (header->read - header->chunkStart < length + 1) {
                    header->state = 2;
                    return;
                }
            } else if (header->read - header->chunkStart - 32 < length + 1) {
                header->state = 2;
                return;
            }
            rest = length % 32;
            if (rest == 0 || request->type == 1)
                header->chunkStart = header->bufferStart;
            else
                header->chunkStart = header->bufferStart + 32 - rest;
            MEM_copy(header->chunkStart, header->requestStart, length);
            marker = header->requestStart;
            marker[0] = 0xFF;
            marker[1] = 0xFF;
            marker[2] = 0xFF;
            marker[3] = 0xFF;
            marker[4] = 8;
            marker[5] = 0;
            marker[6] = 0;
            marker[7] = 0;
            header->requestStart = header->chunkStart;
            header->write = header->requestStart + length;
            if (STREAM_GET32(header->read) == -1) {
                header->read = header->chunkStart;
                space = header->bufferEnd - header->write - 32;
            } else {
                space = header->read - header->write - 1;
            }
        }
    }
    if (space < header->minimum) {
        header->state = 2;
        return;
    }
    request = header->current;
    if (request->type == 1) {
        if (header->position + space > request->size)
            header->requested = request->size - header->position;
        else
            header->requested = space;
        MEM_copy(header->write, request->source, header->requested);
        request->source += header->requested;
        readcallback(0, 0, header);
    } else {
        header->requested = header->minimum;
        header->fileOp = FILESYS_read(header->handle, header->position, header->write, header->requested, priority, header);
        if (header->fileOp)
            FILESYS_callbackop(header->fileOp, readcallback);
    }
}
