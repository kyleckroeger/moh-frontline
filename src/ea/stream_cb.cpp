/* A fragment of EA's streaming layer (stream.cpp, 0x8014bb68): the request
   list, the chunk parser, the file operation callbacks, the buffer refill
   and STREAM_overhead. freerequest unlinks a request from the queue (moving
   the current request on, or back when it was the last) and puts it on the
   free list. parsechunks walks the chunks written for the current request:
   a chunk whose size word has a high byte is a raw read and gets a header
   naming the request; the request's own chunks are padded to 32 bytes; the
   id is matched against the chunk type table (id & mask == value), unknown
   types are marked free (-2) and known ones get their type in the size
   word's high byte and are added to their tap's list and the buffered
   count, leaving greedy mode when the count crosses the threshold. It
   returns 1 when it reaches the request's end chunk, and stops (leaving the
   chunk unclaimed) once the request is cancelled (state 4). opencallback
   records the opened handle and restarts the stream; closecallback reopens
   the stream's file and installs opencallback; readcallback adds the bytes
   read to the file position and the write pointer, parses the new chunks,
   and then starts the next request when this one is finished (its whole
   file read, or a short read, or chunks parsed), marking it done, or
   restarts the stream; startnextrequest moves to the next queued request
   (or marks the stream idle), resets the read position, and switches files
   (closing the open one, or opening the new one) when the request names
   another file, else restarts the stream. restartstream skips consumed
   chunks (an id of -1 wraps to the chunk start, -2 skips its length), frees
   finished requests whose data left the unread part of the ring, finds the
   free space (moving the partial request to the buffer start behind a wrap
   marker when the tail is too short), and then copies the next part of a
   memory request or starts a file read; the stream is stalled (state 2)
   while there is too little room. STREAM_overhead is the memory a stream
   needs besides its buffer; STREAM_create checks the limits (a buffer of at
   least 6 KB, 2..256 requests, 1..16 chunk types, 1..types taps), lays the
   header, request, type and tap tables and the 32-byte aligned buffer out
   in the given memory, picks the minimum read from the buffer size, chains
   the free requests, and returns the first tap as the handle.
   STREAMHEADERtag and REQUESTSTRUCTtag are named by the mangled symbols;
   their members, the other views and the helpers' signatures are
   inferred. */
/* inferred: a stream request (state, type, file size) */
struct REQUESTSTRUCTtag {
    int index;
    int state;
    REQUESTSTRUCTtag* prev;
    REQUESTSTRUCTtag* next;
    int type;
    char filename[256];
    unsigned char* source;
    int size;
    int id;
    unsigned char* start;
};

struct STREAMHEADERtag;

/* inferred: a chunk type match (id & mask == value) */
struct CHUNKTYPEVIEW {
    int mask;
    int value;
    int type;
};

/* inferred: the chunks buffered for one type */
struct CHUNKLISTVIEW {
    STREAMHEADERtag* header;
    int number;
    int size;
    unsigned char* first;
};

struct STREAMHEADERtag {
    int magic;
    unsigned char mutex[28];
    REQUESTSTRUCTtag* requests;
    int requestCount;
    CHUNKTYPEVIEW* types;
    int typeCount;
    CHUNKLISTVIEW* lists;
    int listCount;
    unsigned char* bufferStart;
    unsigned char* chunkStart;
    unsigned char* bufferEnd;
    int state;
    int unknown48;
    int priority;
    int threshold;
    int greedy;
    int buffered;
    unsigned char* read;
    unsigned char* requestStart;
    unsigned char* write;
    REQUESTSTRUCTtag* first;
    REQUESTSTRUCTtag* current;
    REQUESTSTRUCTtag* last;
    REQUESTSTRUCTtag* free;
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
void MEM_clear(void*, int);
void MUTEX_create(void*);
}

static void restartstream(STREAMHEADERtag*, int);
static void startnextrequest(STREAMHEADERtag*, int);

#define STREAM_GET32(p) (((p)[3] << 24) | ((p)[2] << 16) | ((p)[1] << 8) | (p)[0])

/* inferred: stores a little-endian word */
static inline void put32(unsigned char* p, unsigned int value) {
    p[0] = value;
    p[1] = value >> 8;
    p[2] = value >> 16;
    p[3] = value >> 24;
}

static void freerequest(STREAMHEADERtag* header, REQUESTSTRUCTtag* request) {
    if (request == header->first)
        header->first = request->next;
    else
        request->prev->next = request->next;
    if (request == header->last)
        header->last = request->prev;
    else
        request->next->prev = request->prev;
    if (request == header->current) {
        if (!request->next)
            header->current = request->prev;
        else
            header->current = request->next;
    }
    request->state = 0;
    request->next = header->free;
    header->free = request;
}

static inline int chunktype(STREAMHEADERtag* header, int id) {
    int i;

    for (i = 0; i < header->typeCount; i++) {
        CHUNKTYPEVIEW* entry = &header->types[i];

        if (entry->value == (id & entry->mask))
            return entry->type;
    }
    return -2;
}

static int parsechunks(STREAMHEADERtag* header) {
    unsigned char* chunk;
    REQUESTSTRUCTtag* request;
    int size;
    int type;
    int done;

    request = header->current;
    while (header->write - (chunk = header->requestStart) >= 8) {
        size = STREAM_GET32(chunk + 4);
        if (size & 0xFF000000) {
            put32(chunk, request->id);
            chunk[4] = 8;
            chunk[5] = 0;
            chunk[6] = 0;
            chunk[7] = 0;
            size = 8;
        }
        if (header->requestStart + size > header->write)
            break;
        if (request->id == STREAM_GET32(chunk)) {
            int rest = (unsigned int)header->requestStart & 31;

            size = ((size + rest + 31) & ~31) - rest;
        }
        type = chunktype(header, STREAM_GET32(chunk));
        if (type < 0) {
            MUTEX_lock(header->mutex);
            done = request->state == 4;
            if (!done) {
                chunk[0] = 0xFE;
                chunk[1] = 0xFF;
                chunk[2] = 0xFF;
                chunk[3] = 0xFF;
                header->requestStart += size;
            }
            MUTEX_unlock(header->mutex);
        } else {
            int word = size | (type << 24);

            put32(chunk + 4, word);
            MUTEX_lock(header->mutex);
            done = request->state == 4;
            if (!done) {
                CHUNKLISTVIEW* list = &header->lists[type - 1];
                int before;
                int after;
                int threshold;

                list->size += size;
                if (list->size == size)
                    list->first = chunk;
                header->requestStart += size;
                before = header->buffered;
                after = before + size;
                header->buffered = after;
                threshold = header->threshold;
                if (before < threshold && after >= threshold)
                    header->greedy = 0;
            }
            MUTEX_unlock(header->mutex);
        }
        if (done) {
            if (request->id != STREAM_GET32(chunk)) {
                int rest = (unsigned int)header->requestStart & 31;

                size = ((size + rest + 31) & ~31) - rest;
                put32(chunk + 4, size | (type << 24));
            }
            break;
        }
        if (request->id == STREAM_GET32(chunk))
            return 1;
    }
    return 0;
}

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
    REQUESTSTRUCTtag* request = header->current;
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
    REQUESTSTRUCTtag* request = 0;
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

static void restartstream(STREAMHEADERtag* header, int priority) {
    int space;
    int length;
    REQUESTSTRUCTtag* request;

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
        REQUESTSTRUCTtag* next = header->first->next;
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
        freerequest(header, header->first);
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

/* inferred: the bytes a stream needs besides its buffer (header, request,
   chunk type and tap tables, and 32 for aligning the buffer) */
#define STREAM_OVERHEAD(requests, types, taps) \
    (int)(sizeof(STREAMHEADERtag) + 32 + (requests) * sizeof(REQUESTSTRUCTtag) + (types) * sizeof(CHUNKTYPEVIEW) + (taps) * sizeof(CHUNKLISTVIEW))

extern "C" int STREAM_overhead(int requests, int types, int taps) {
    return STREAM_OVERHEAD(requests, types, taps);
}

extern "C" int STREAM_create(int requests, int types, int taps, void* memory, int size) {
    STREAMHEADERtag* header;
    int buffer;
    int i;

    if (size - STREAM_OVERHEAD(requests, types, taps) < 6144)
        return 0;
    if (requests < 2)
        return 0;
    if (requests > 256)
        return 0;
    if (types < 1 || types > 16)
        return 0;
    if (taps < 1 || taps > types)
        return 0;
    header = (STREAMHEADERtag*)memory;
    header->magic = 0x4D525453;
    MUTEX_create(header->mutex);
    header->requests = (REQUESTSTRUCTtag*)(header + 1);
    header->requestCount = requests;
    header->types = (CHUNKTYPEVIEW*)(header->requests + requests);
    header->typeCount = types;
    header->lists = (CHUNKLISTVIEW*)(header->types + types);
    header->listCount = taps;
    header->bufferStart = (unsigned char*)(((unsigned int)(header->lists + taps) & ~31) + 32);
    header->chunkStart = header->bufferStart;
    header->bufferEnd = (unsigned char*)memory + size;
    header->state = 0;
    header->unknown48 = 150;
    header->priority = 50;
    header->threshold = 0;
    header->greedy = 0;
    header->buffered = 0;
    header->read = header->chunkStart;
    header->requestStart = header->chunkStart;
    header->write = header->chunkStart;
    header->first = 0;
    header->current = 0;
    header->last = 0;
    header->free = header->requests;
    MEM_clear(header->filename, 255);
    header->handle = 0;
    buffer = size - STREAM_OVERHEAD(requests, types, taps);
    if (buffer < 16384)
        header->minimum = 2048;
    else if (buffer < 32768)
        header->minimum = 4096;
    else if (buffer < 65536)
        header->minimum = 8192;
    else if (buffer < 131072)
        header->minimum = 16384;
    else
        header->minimum = 32768;
    for (i = 0; i < requests; i++) {
        REQUESTSTRUCTtag* request = &header->requests[i];

        request->index = i;
        request->state = 0;
        request->next = &header->requests[i + 1];
    }
    header->requests[requests - 1].next = 0;
    for (i = 0; i < types; i++) {
        CHUNKTYPEVIEW* type = &header->types[i];

        type->mask = 0;
        type->value = 0;
        type->type = 1;
    }
    for (i = 0; i < taps; i++) {
        CHUNKLISTVIEW* tap = &header->lists[i];

        tap->header = header;
        tap->number = i + 1;
        tap->size = 0;
    }
    return (int)header->lists;
}
