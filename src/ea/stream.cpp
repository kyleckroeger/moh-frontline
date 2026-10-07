// EA's streaming layer, the end of the file: getting a tap's next chunk
// (clearing the type from its size word, taking its bytes from the tap's
// count, and finding the tap's following chunk, skipping wrap markers),
// releasing a chunk (marking it free, returning its bytes to the buffer,
// leaving greedy mode or restarting a stalled stream) and the queries for the
// stream's table, state, end-of-stream test and buffered size. Chunk headers
// are little-endian words (an id, -2 when free, then the size with the type
// in its high byte). Every public function validates its (integer) handle
// through the same inline check: a non-null handle whose header carries the
// stream magic. STREAMHEADERtag is named by the mangled symbols; the header
// and handle layouts and the inline helper are inferred. Earlier parts of
// the file are in stream_cb.cpp, stream_prio.cpp and stream_greedy.cpp.
struct STREAMHEADERtag {
    int magic;
    unsigned char mutex[44];
    unsigned char* taps;
    int tapCount;
    int bufferStart;
    unsigned char* chunkStart;
    int bufferEnd;
    int state;
    int priority;
    int greedyPriority;
    int greedyThreshold;
    int greedy;
    int buffered;
    unsigned char unknown5c[292];
    int fileOp;
};

extern "C" {
void MUTEX_lock(void*);
void MUTEX_unlock(void*);
int FILESYS_priorityop(int, int);
}

// File-local in the original (defined earlier in the file).
void restartstream(STREAMHEADERtag*, int);

struct STREAMHANDLEVIEW {
    STREAMHEADERtag* header;
    int tag;
    int table;
    unsigned char* chunk;
};

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

#define STREAM_GET32(p) (((p)[3] << 24) | ((p)[2] << 16) | ((p)[1] << 8) | (p)[0])

extern "C" unsigned char* STREAM_get(int handle) {
    STREAMHEADERtag* header;
    STREAMHANDLEVIEW* stream;
    unsigned char* chunk;
    unsigned int size;
    int remaining;
    int info;
    unsigned char* next;
    int tag;
    if (validhandle(handle, &stream, &header) != 0)
        return 0;
    if (stream->table == 0)
        return 0;
    chunk = stream->chunk;
    size = STREAM_GET32(chunk + 4) & 0xFFFFFF;
    chunk[4] = size;
    chunk[5] = size >> 8;
    chunk[6] = size >> 16;
    chunk[7] = size >> 24;
    MUTEX_lock(header->mutex);
    remaining = stream->table - size;
    stream->table = remaining;
    MUTEX_unlock(header->mutex);
    if (remaining > 0) {
        tag = stream->tag << 24;
        next = chunk + size;
        while (((info = STREAM_GET32(next + 4)) & ~0xFFFFFF) != tag) {
            if (STREAM_GET32(next) == -1)
                next = header->chunkStart;
            else
                next += info & 0xFFFFFF;
        }
        stream->chunk = next;
    }
    return chunk;
}

extern "C" void STREAM_release(int handle, unsigned char* chunk) {
    STREAMHANDLEVIEW* stream;
    STREAMHEADERtag* header;
    int size;
    int before;
    int after;
    int state;
    if (validhandle(handle, &stream, &header) != 0)
        return;
    if (chunk < header->chunkStart || chunk > (unsigned char*)header->bufferEnd - 8)
        return;
    if (STREAM_GET32(chunk) == -2)
        return;
    chunk[0] = 0xFE;
    chunk[1] = 0xFF;
    chunk[2] = 0xFF;
    chunk[3] = 0xFF;
    size = STREAM_GET32(chunk + 4);
    MUTEX_lock(header->mutex);
    before = header->buffered;
    after = before - size;
    header->buffered = after;
    MUTEX_unlock(header->mutex);
    if (before >= header->greedyThreshold && after < header->greedyThreshold) {
        header->greedy = 1;
        if (header->state == 1)
            FILESYS_priorityop(header->fileOp, header->greedyPriority);
    }
    MUTEX_lock(header->mutex);
    state = header->state;
    if (state == 2)
        header->state = 1;
    MUTEX_unlock(header->mutex);
    if (state == 2) {
        if (header->greedy)
            restartstream(header, header->greedyPriority);
        else
            restartstream(header, header->priority);
    }
}

extern "C" void* STREAM_gettable(int handle) {
    STREAMHANDLEVIEW* stream;
    STREAMHEADERtag* header;
    if (validhandle(handle, &stream, &header) != 0)
        return 0;
    return (void*)stream->table;
}

extern "C" int STREAM_state(int handle) {
    STREAMHANDLEVIEW* stream;
    STREAMHEADERtag* header;
    if (validhandle(handle, &stream, &header) != 0)
        return 0;
    return header->state;
}

extern "C" int STREAM_isendofstream(int handle) {
    STREAMHEADERtag* header;
    STREAMHANDLEVIEW* stream;
    int result;
    if (validhandle(handle, &stream, &header) != 0)
        return 0;
    result = 0;
    if (header->state == 0 && stream->table == 0)
        result = 1;
    return result;
}

extern "C" int STREAM_buffersize(int handle) {
    STREAMHANDLEVIEW* stream;
    STREAMHEADERtag* header;
    if (validhandle(handle, &stream, &header) != 0)
        return 0;
    return header->bufferEnd - header->bufferStart;
}
