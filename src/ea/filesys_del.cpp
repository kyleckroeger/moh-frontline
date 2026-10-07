// A fragment of EA's filesys.cpp (0x801494b4): removing a big file and
// running a function atomically. iFILE_delbigclosecallback completes the
// close and runs the pending remove operation. FILESYS_delbig queues a
// remove operation that fails (error 1, status -2) while an open handle
// still refers to the big file, and with error 1 when the file is not
// linked on the device; it unlinks and frees the file's header buffer and
// record and closes its handle, the close's callback running the remove.
// FILESYS_atomic calls a function with the device's third word set to the
// first argument, then runs the queue. FILESYS_addbig before this unit is
// not part of it. The views are those of filesys.cpp (members inferred;
// FILEOPERATION_def and HANDLE_def are named by the mangled helpers);
// FILESYS_close and FILESYS_callbackop are defined in the filesys.cpp unit
// and inlined here, so this fragment keeps inline copies of their bodies.
extern "C" {
int FILESYS_completeop(int);
}

struct BIGFILEVIEW;

struct HANDLE_def {
    int unknown000;
    unsigned char unknown004;
    char name[255];
    int unknown104;
    BIGFILEVIEW* big;
    unsigned char unknown10c[56];
};

struct FILEOPERATION_def;

FILEOPERATION_def* reserveop();
void iFILE_perror(FILEOPERATION_def*);
void iFILE_ExecCommand(FILEOPERATION_def*);

struct FILESYSOPTSVIEW {
    int unknown00;
    void* (*malloc)(const char*, int, int);
    int (*mfree)(void*);
};

/* inferred: an operation handle (serial in the high half, slot in the low
   byte) and the 48-byte operation record it names */
union FILEOPID {
    int value;
    struct {
        unsigned short serial;
        unsigned char unknown2;
        unsigned char slot;
    } parts;
};

typedef void (*FILEOPCALLBACK)(int, int, void*);

struct FILEOPERATION_def {
    FILEOPID id;
    int cancelled;
    volatile int status;
    int error;
    int priority;
    void* callbackData;
    int unknown18;
    int unknown1c;
    void* buffer;
    HANDLE_def* handle;
    FILEOPCALLBACK callback;
    FILEOPERATION_def* next;
};

/* inferred: a big file being added (its header buffer, the bytes read so
   far, the allocation flags, its handle and the pending add operation),
   linked on the device once loaded */
struct BIGFILEVIEW {
    void* buffer;
    int size;
    int memflags;
    HANDLE_def* handle;
    FILEOPERATION_def* op;
    BIGFILEVIEW* next;
};

struct FILEDEVICEVIEW {
    int operations;
    int handles;
    int unknown08;
    int unknown0c;
    int callbackDepth;
    FILEOPERATION_def* active;
    union {
        char* memory;
        FILEOPERATION_def* ops;
    };
    char* handleMemory;
    FILEOPERATION_def* queue;
    BIGFILEVIEW* links;
};

extern FILESYSOPTSVIEW gFileSysOpts;

extern FILEDEVICEVIEW gFileDevice;

static inline FILEOPERATION_def* getop(int op) {
    FILEOPID id;

    id.value = op;
    return &gFileDevice.ops[id.parts.slot];
}

extern "C" inline void FILESYS_callbackop(int op, FILEOPCALLBACK callback) {
    FILEOPERATION_def* record = getop(op);

    record->callback = callback;
    if (record->status) {
        gFileDevice.callbackDepth++;
        callback(op, record->status, record->callbackData);
        gFileDevice.callbackDepth--;
    }
}

extern "C" inline int FILESYS_close(HANDLE_def* handle, int priority, void* callbackData) {
    FILEOPERATION_def* op = reserveop();
    BIGFILEVIEW* link = gFileDevice.links;

    op->id.parts.unknown2 = 3;
    op->priority = priority;
    op->callbackData = callbackData;
    op->handle = handle;
    for (; link; link = link->next) {
        if (link->handle == handle) {
            op->error = 3;
            iFILE_perror(op);
            break;
        }
    }
    iFILE_ExecCommand(op);
    return op->id.value;
}

void iFILE_delbigclosecallback(int op, int, void* data) {
    FILESYS_completeop(op);
    iFILE_ExecCommand((FILEOPERATION_def*)data);
}

extern "C" int FILESYS_delbig(HANDLE_def* handle, int priority, void* callbackData) {
    BIGFILEVIEW* prev;
    BIGFILEVIEW* link = gFileDevice.links;
    FILEOPERATION_def* op;
    HANDLE_def* other;
    int i;

    prev = 0;
    op = reserveop();
    other = (HANDLE_def*)gFileDevice.handleMemory;
    op->id.parts.unknown2 = 10;
    op->callbackData = callbackData;
    op->priority = priority;
    if (handle->unknown000 == 1)
        op->error = 1;
    for (i = gFileDevice.handles; i > 0; i--, other++) {
        if (other && other->unknown000 == 1 && other->big->handle == handle) {
            op->status = -2;
            op->error = 1;
            break;
        }
    }
    for (; link && link->handle != handle; link = link->next)
        prev = link;
    if (!link)
        op->error = 1;
    if (prev)
        prev->next = link->next;
    else
        gFileDevice.links = gFileDevice.links->next;
    gFileSysOpts.mfree(link->buffer);
    gFileSysOpts.mfree(link);
    FILESYS_callbackop(FILESYS_close(handle, priority, op), iFILE_delbigclosecallback);
    return op->id.value;
}

extern "C" int FILESYS_atomic(int (*function)(int, int), int, int a, int b) {
    int result;
    int saved = gFileDevice.unknown08;

    gFileDevice.unknown08 = a;
    result = function(a, b);
    gFileDevice.unknown08 = saved;
    iFILE_ExecCommand(0);
    return result;
}
