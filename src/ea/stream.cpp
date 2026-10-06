// EA's streaming layer, the end of the file: releasing a chunk (marking it
// free, returning its bytes to the buffer, leaving greedy mode or restarting
// a stalled stream) and the queries for the stream's table, state,
// end-of-stream test and buffered size. Chunk headers are little-endian words
// (an id, -2 when free, then the size). Every public function
// validates its (integer) handle through the same inline check: a non-null
// handle whose header carries the stream magic. STREAMHEADERtag is named by
// the mangled symbols; the header and handle layouts and the inline helper
// are inferred. The priority and greedy setters earlier in the file are
// drafted in scratch/lib/stream_wip.cpp (STREAM_setgreedylevel is off).
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
    int unknown04;
    void* table;
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
    return stream->table;
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
    if (header->state == 0 && (int)stream->table == 0)
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
