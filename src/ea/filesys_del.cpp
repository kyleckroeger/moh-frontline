// A fragment of EA's filesys.cpp (0x801494b4) to the end of the file:
// removing a big file, running a function atomically, the command queue and
// the record allocators. iFILE_delbigclosecallback completes the close and
// runs the pending remove operation. FILESYS_delbig queues a remove
// operation that fails (error 1, status -2) while an open handle still
// refers to the big file, and with error 1 when the file is not linked on
// the device; it unlinks and frees the file's header buffer and record and
// closes its handle, the close's callback running the remove. FILESYS_atomic
// calls a function with the device's third word set to the first argument,
// then runs the queue. iFILE_ExecCommand queues an operation by priority and,
// when the device is idle, runs the first one the priority limit allows:
// opening (a plain file, or an entry of a linked big file named
// "big|entry" or "|entry"), checking existence, closing, reading (through
// the big file's handle at the entry's offset), writing, querying the size
// or deleting. iFILE_CommandCompleteCallback sets the active operation's
// status, calls its callback and runs the queue again. reserveop (with a
// serial that skips 0), reservehandle, freeop and freehandle take records
// under the mutex and spin when none is free; iFILE_perror does nothing.
// FILESYS_addbig before this unit is not part of it. The views are those of
// filesys.cpp (members inferred; FILEOPERATION_def and HANDLE_def are named
// by the mangled helpers); FILESYS_close and FILESYS_callbackop are defined
// in the filesys.cpp unit and inlined here, so this fragment keeps inline
// copies of their bodies.
struct OSMutex {
    unsigned char unknown00[24];
};

extern "C" {
void OSLockMutex(OSMutex*);
void OSUnlockMutex(OSMutex*);
char* strchr(const char*, int);
char* strncpy(char*, const char*, unsigned long);
char* strcpy(char*, const char*);
int strcmp(const char*, const char*);
void MEM_fill(void*, int, int);
void* BIG_locateentryz(void*, const char*, int, int*, int*);
int FILESYS_completeop(int);
}

struct BIGFILEVIEW;

struct HANDLE_def {
    int unknown000;
    unsigned char unknown004;
    char name[255];
    int unknown104;
    BIGFILEVIEW* big;
    int unknown10c;
    unsigned char unknown110[52];
};

struct FILEOPERATION_def;

static FILEOPERATION_def* reserveop();
static void iFILE_ExecCommand(FILEOPERATION_def*);
static void iFILE_perror(FILEOPERATION_def*);
void iFILE_CommandCompleteCallback(int);
int openfile(const char*, unsigned int, HANDLE_def*);
int closefile(HANDLE_def*);
int getfilesize(HANDLE_def*);
void readfile(HANDLE_def*, void*, int, int);
void writefile(HANDLE_def*, void*, int, int);
void deletefile(const char*);
static void freehandle(HANDLE_def*);

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
    union {
        char* handleMemory;
        HANDLE_def* handleList;
    };
    FILEOPERATION_def* queue;
    BIGFILEVIEW* links;
};

extern FILESYSOPTSVIEW gFileSysOpts;

extern OSMutex FileMutex;
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
    other = gFileDevice.handleList;
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

static void iFILE_ExecCommand(FILEOPERATION_def* op) {
    char bigname[256];
    char path[256];
    int size;
    int offset;

    OSLockMutex(&FileMutex);
    if (op) {
        FILEOPERATION_def* prev = 0;

        op->next = gFileDevice.queue;
        while (op->next && op->next->priority <= op->priority) {
            prev = op->next;
            op->next = op->next->next;
        }
        if (prev)
            prev->next = op;
        else
            gFileDevice.queue = op;
        gFileDevice.unknown0c++;
    }
    if (gFileDevice.active) {
        OSUnlockMutex(&FileMutex);
        return;
    }
    op = gFileDevice.queue;
    if (op && op->priority <= gFileDevice.unknown08) {
        gFileDevice.queue = op->next;
        gFileDevice.unknown0c--;
    } else {
        op = 0;
    }
    gFileDevice.active = op;
    OSUnlockMutex(&FileMutex);
    if (op && !op->cancelled) {
        switch (op->id.parts.unknown2) {
        case 2:
        case 8: {
            int flags;
            int length;
            int done;
            BIGFILEVIEW* big;

            path[0] = 0;
            if (strchr(op->handle->name, '|')) {
                if (op->handle->name[0] == '|') {
                    flags = 2;
                } else {
                    length = strchr(op->handle->name, '|') - op->handle->name;
                    bigname[0] = 0;
                    strncpy(bigname, op->handle->name, length);
                    bigname[length] = 0;
                    flags = 4;
                }
                strcpy(path, strchr(op->handle->name, '|') + 1);
            } else {
                flags = 1;
                if (op->unknown18 & 1) {
                    flags |= 2;
                    strcpy(path, op->handle->name);
                }
            }
            done = 0;
            if (flags & 1) {
                if (openfile(op->handle->name, op->unknown18, op->handle)) {
                    done = 1;
                    if (op->id.parts.unknown2 == 8)
                        closefile(op->handle);
                    else
                        op->handle->unknown104 = getfilesize(op->handle);
                }
            }
            if (!done && (flags & 6)) {
                flags &= 4;
                big = gFileDevice.links;
                while (big && !done) {
                    if (flags && strcmp(big->handle->name, bigname)) {
                        big = big->next;
                    } else {
                        if (BIG_locateentryz(big->buffer, path, 0, &offset, &size)) {
                            done = 1;
                            op->handle->big = big;
                            op->handle->unknown104 = size;
                            op->handle->unknown10c = offset;
                            op->handle->unknown000 = 1;
                        }
                        big = big->next;
                    }
                }
            }
            if (op->id.parts.unknown2 == 8) {
                freehandle(op->handle);
                op->unknown18 = done;
            }
            iFILE_CommandCompleteCallback(done);
            break;
        }
        case 3:
            op->error = 1;
            if (op->handle) {
                if (op->handle->unknown000 != 1)
                    op->error = closefile(op->handle);
                freehandle(op->handle);
                op->handle = 0;
            }
            iFILE_CommandCompleteCallback(op->error == 0);
            break;
        case 4:
            if (op->unknown1c > 0) {
                if (op->handle->unknown000 == 1)
                    readfile(op->handle->big->handle, op->buffer, op->handle->unknown10c + op->unknown18, op->unknown1c);
                else
                    readfile(op->handle, op->buffer, op->unknown18, op->unknown1c);
            } else {
                iFILE_CommandCompleteCallback(1);
            }
            break;
        case 5:
            writefile(op->handle, op->buffer, op->unknown18, op->unknown1c);
            iFILE_CommandCompleteCallback(1);
            break;
        case 6:
            op->unknown18 = op->handle->unknown104;
            iFILE_CommandCompleteCallback(1);
            break;
        case 7:
        case 9:
        case 10:
            iFILE_CommandCompleteCallback(op->error == 0);
            break;
        case 11:
            deletefile(op->handle->name);
            iFILE_CommandCompleteCallback(1);
            break;
        }
    }
}

void iFILE_CommandCompleteCallback(int ok) {
    FILEOPERATION_def* op = gFileDevice.active;

    if (op) {
        if (op->cancelled)
            op->status = -1;
        else
            op->status = ok ? 1 : -2;
        gFileDevice.active = 0;
        if (op->callback) {
            gFileDevice.callbackDepth++;
            op->callback(op->id.value, op->status, op->callbackData);
            gFileDevice.callbackDepth--;
        }
        if (gFileDevice.callbackDepth == 0)
            iFILE_ExecCommand(0);
    }
}

static void iFILE_perror(FILEOPERATION_def*) {
}

static FILEOPERATION_def* reserveop() {
    static unsigned short opcount;
    static char init;
    int i;

    if (!init) {
        opcount = 0;
        init = 1;
    }
    OSLockMutex(&FileMutex);
    for (i = 0; i < gFileDevice.operations; i++) {
        if (gFileDevice.ops[i].id.parts.unknown2 == 0) {
            gFileDevice.ops[i].id.parts.unknown2 = 1;
            gFileDevice.ops[i].id.parts.slot = i;
            if (++opcount == 0)
                opcount = 1;
            gFileDevice.ops[i].id.parts.serial = opcount;
            break;
        }
    }
    OSUnlockMutex(&FileMutex);
    if (i == gFileDevice.operations)
        for (;;)
            ;
    return &gFileDevice.ops[i];
}

static void freeop(FILEOPERATION_def* op) {
    OSLockMutex(&FileMutex);
    MEM_fill(op, 0, sizeof(FILEOPERATION_def));
    OSUnlockMutex(&FileMutex);
}

static HANDLE_def* reservehandle() {
    int i;

    OSLockMutex(&FileMutex);
    for (i = 0; i < gFileDevice.handles; i++) {
        if (gFileDevice.handleList[i].unknown004 == 0) {
            gFileDevice.handleList[i].unknown004 = 1;
            break;
        }
    }
    OSUnlockMutex(&FileMutex);
    if (i == gFileDevice.handles)
        for (;;)
            ;
    return &gFileDevice.handleList[i];
}

static void freehandle(HANDLE_def* handle) {
    OSLockMutex(&FileMutex);
    MEM_fill(handle, 0, sizeof(HANDLE_def));
    OSUnlockMutex(&FileMutex);
}

extern "C" void FILESYS_setmemcallbacks(void* (*malloc)(const char*, int, int), int (*mfree)(void*)) {
    gFileSysOpts.malloc = malloc;
    gFileSysOpts.mfree = mfree;
}
