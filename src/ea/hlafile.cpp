// EA's asynchronous file layer, from ASYNCFILE_restore to ASYNCFILE_getstatus:
// shutting down (cancel every request, wait until none has a file operation
// running, free the table and destroy the mutex), queueing a whole-file load
// or a read (take a request from the free list, give it a fresh id, start the
// file-system operation and its completion callback) and reporting whether a
// request is still busy. The function and static names come from the
// symbols; the 48-byte request layout, the options view and the inline
// helpers are inferred; the operation handle is volatile (completion
// callbacks clear it, and the target reloads it). The file's small statics
// and mutex are defined here in their original order.
extern "C" {
void MUTEX_lock(void*);
void MUTEX_unlock(void*);
void MUTEX_destroy(void*);
bool THREAD_iscurrent(int);
void SYNCTASK_run(int);
void THREAD_yield(int);
int FILESYS_open(const char*, int, int, void*);
int FILESYS_read(int, void*, int, int, int, void*);
void FILESYS_callbackop(int, void (*)(int, int, void*));
void ASYNCFILE_cancel(int);
}

// File-local in the original (defined earlier in the file).
void loadfileopencallback(int, int, void*);
void readfilereadcallback(int, int, void*);

struct FILESYSOPTSVIEW {
    int unknown00;
    void* (*malloc)(const char*, int, int);
    int (*mfree)(void*);
};

extern FILESYSOPTSVIEW gFileSysOpts;

struct ASYNCREQUESTVIEW {
    int id;
    ASYNCREQUESTVIEW* next;
    int unknown08;
    int unknown0c;
    int unknown10;
    int unknown14;
    int unknown18;
    volatile int op;
    int file;
    void* buffer;
    int size;
    int position;
};

static int numrequests;
static ASYNCREQUESTVIEW* request;
static ASYNCREQUESTVIEW* freequeue[2];
static int requestidcounter;
static unsigned char mutex[28];

static inline ASYNCREQUESTVIEW* allocrequest() {
    ASYNCREQUESTVIEW* r;
    MUTEX_lock(mutex);
    if (freequeue[0] == 0) {
        r = 0;
    } else {
        r = freequeue[0];
        freequeue[0] = r->next;
    }
    MUTEX_unlock(mutex);
    return r;
}

static inline void newrequestid(ASYNCREQUESTVIEW* r) {
    requestidcounter += 256;
    if (requestidcounter == 0)
        requestidcounter = 256;
    r->id = (r->id & 0xFF) | requestidcounter;
}

extern "C" void ASYNCFILE_restore() {
    int busy;
    int i;
    if (request) {
        for (i = 0; i < numrequests; i++)
            ASYNCFILE_cancel(request[i].id);
        do {
            busy = 0;
            for (i = 0; i < numrequests; i++) {
                if (request[i].op != 0)
                    busy = 1;
            }
            if (busy) {
                if (THREAD_iscurrent(0))
                    SYNCTASK_run(0);
                THREAD_yield(0);
            }
        } while (busy);
        gFileSysOpts.mfree(request);
        MUTEX_destroy(mutex);
        request = 0;
    }
}

extern "C" int ASYNCFILE_load(const char* name, int size) {
    ASYNCREQUESTVIEW* r = allocrequest();
    if (r == 0)
        return 0;
    newrequestid(r);
    r->unknown08 = 0;
    r->unknown0c = 0;
    r->unknown10 = 0;
    r->unknown14 = 1;
    r->unknown18 = 0;
    r->buffer = 0;
    r->size = size;
    r->position = 0;
    r->op = FILESYS_open(name, 1, 100, r);
    if (r->op == 0)
        return 0;
    FILESYS_callbackop(r->op, loadfileopencallback);
    return r->id;
}

extern "C" int ASYNCFILE_read(int file, void* buffer, int position, int size) {
    ASYNCREQUESTVIEW* r = allocrequest();
    if (r == 0)
        return 0;
    newrequestid(r);
    r->unknown08 = 0;
    r->unknown0c = 0;
    r->unknown10 = 0;
    r->unknown14 = 0;
    r->unknown18 = 0;
    r->file = file;
    r->buffer = buffer;
    r->size = size;
    r->position = position;
    if (size > 0x8000)
        size = 0x8000;
    r->op = FILESYS_read(file, buffer, position, size, 100, r);
    if (r->op == 0)
        return 0;
    FILESYS_callbackop(r->op, readfilereadcallback);
    return r->id;
}

static inline ASYNCREQUESTVIEW* findrequest(int id) {
    int index = id & 0xFF;
    ASYNCREQUESTVIEW* r;
    if (id < 256 || index >= numrequests)
        return 0;
    r = &request[index];
    if (r->id != id)
        return 0;
    return r;
}

extern "C" int ASYNCFILE_getstatus(int id) {
    ASYNCREQUESTVIEW* r;
    int status;
    MUTEX_lock(mutex);
    r = findrequest(id);
    if (r == 0)
        status = -1;
    else
        status = r->op == 0;
    MUTEX_unlock(mutex);
    return status;
}
