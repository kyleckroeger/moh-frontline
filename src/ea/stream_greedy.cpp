// A fragment of EA's stream.cpp (0x8014ce78): STREAM_setgreedystate (stores
// the greedy state and, when it is set while the stream is reading, raises the
// file operation's priority) and STREAM_taphandle (the 16-byte tap entry for a
// 1-based tap number, or null). The file name is this project's; the original
// record is stream.cpp (src/ea/stream.cpp holds STREAM_release onward) and
// the functions around these are not reconstructed. The handle check is the
// same inferred inline as in stream.cpp; STREAMHEADERtag is named by the
// mangled symbols and the header and handle layouts are inferred.
// EA's streaming layer. Every public function validates the handle through
// the same inline check (non-null handle whose header carries the stream
// magic). STREAMHEADERtag is named by the mangled symbols; the header and
// handle layouts, the magic's meaning and the inline helper are inferred.
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

extern "C" int FILESYS_priorityop(int, int);

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

extern "C" void STREAM_setgreedylevel(int handle, int level);

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


