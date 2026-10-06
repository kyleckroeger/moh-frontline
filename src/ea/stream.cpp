// EA's streaming layer, the queries at the end of the file: the stream's
// table, state, end-of-stream test and buffered size. Every public function
// validates its (integer) handle through the same inline check: a non-null
// handle whose header carries the stream magic. STREAMHEADERtag is named by
// the mangled symbols; the header and handle layouts and the inline helper
// are inferred. The priority and greedy setters earlier in the file are
// drafted in scratch/lib/stream_wip.cpp (STREAM_setgreedylevel is off).
struct STREAMHEADERtag {
    int magic;
    unsigned char unknown04[44];
    unsigned char* taps;
    int tapCount;
    int bufferStart;
    int unknown3c;
    int bufferEnd;
    int state;
    int priority;
    int priorityClass;
    int greedyLevel;
    int greedyState;
    int greedyThreshold;
    unsigned char unknown5c[292];
    int fileOp;
};

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
