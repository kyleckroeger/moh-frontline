// Synchronous file operations on top of the asynchronous FILESYS layer. Block
// transfers are split into chunks of at most 0x8000 bytes; the completion
// callback queues the next chunk, and the caller waits until none is left.
// The context layout is inferred from the stack offsets; its type and member
// names are not original.
typedef int (*SyncIOFunc)(int, int, void*, int, int, void*);

struct SyncIO {
    volatile int priority;
    volatile int handle;
    volatile int offset;
    volatile int remaining;
    volatile int total;
    volatile int chunk;
    char* buffer;
    SyncIOFunc func;
    int op;
};

extern "C" {
int FILESYS_completeop(int);
int FILESYS_callbackop(int, void (*)(int, int, void*));
int FILESYS_waitop(int);
int FILESYS_opstatus(int);
int FILESYS_open(const char*, int, int, int);
int FILESYS_read(int, int, void*, int, int, void*);
}

static void synccallback(int op, int status, void* data) {
    SyncIO* io = (SyncIO*)data;
    int done = FILESYS_completeop(op);

    io->op = 0;
    if (status == 1) {
        io->offset += done;
        io->total += done;
        io->buffer += done;
        if (done < io->chunk)
            io->remaining = 0;
        else
            io->remaining -= done;
        if (io->remaining > 0) {
            if (io->remaining > 0x8000)
                io->chunk = 0x8000;
            else
                io->chunk = io->remaining;
            io->op = io->func(io->handle, io->offset, io->buffer, io->chunk, io->priority, (void*)io);
            if (io->op)
                FILESYS_callbackop(io->op, synccallback);
            else
                io->remaining = 0;
        }
    } else {
        io->remaining = 0;
    }
}

static int syncblockio(int handle, int offset, void* buffer, int size, int priority, SyncIOFunc func) {
    SyncIO io;

    io.priority = priority;
    io.handle = handle;
    io.offset = offset;
    io.remaining = size;
    io.total = 0;
    io.chunk = size;
    io.buffer = (char*)buffer;
    io.func = func;
    if (io.chunk > 0x8000)
        io.chunk = 0x8000;
    if ((io.op = func(handle, offset, io.buffer, io.chunk, priority, (void*)&io)) != 0) {
        FILESYS_callbackop(io.op, synccallback);
        do {
            FILESYS_waitop(io.op);
        } while (io.remaining || io.op);
    }
    return io.total;
}

extern "C" bool FILESYS_opensync(const char* name, int mode, int priority, int* handle) {
    bool result = false;
    int op = FILESYS_open(name, mode, priority, 0);

    if (op) {
        FILESYS_waitop(op);
        result = FILESYS_opstatus(op) == 1;
        *handle = FILESYS_completeop(op);
    } else {
        *handle = 0;
    }
    return result;
}

// The original does not inline syncblockio here; the pragma reproduces that.
#pragma dont_inline on
extern "C" int FILESYS_readsync(int handle, int offset, void* buffer, int size, int priority) {
    return syncblockio(handle, offset, buffer, size, priority, FILESYS_read);
}
#pragma dont_inline reset
