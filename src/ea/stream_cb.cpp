/* A fragment of EA's streaming layer (stream.cpp, 0x8014bee0): the file
   operation callbacks. opencallback records the opened handle and restarts
   the stream; closecallback reopens the stream's file and installs
   opencallback; readcallback adds the bytes read to the file position and
   the buffered total, parses the new chunks, and then starts the next
   request when this one is finished (its whole file read, or a short read,
   or chunks parsed), marking it done, or restarts the stream;
   startnextrequest moves to the next queued request (or marks the stream
   idle), resets the read position, and switches files (closing the open
   one, or opening the new one) when the request names another file, else
   restarts the stream. STREAMHEADERtag
   is named by the mangled symbols; its members, the request view and the
   helpers' signatures are inferred. */
/* inferred: a stream request (state, type, file size) */
struct REQUESTVIEW {
    unsigned char unknown00[4];
    int state;
    unsigned char unknown08[4];
    REQUESTVIEW* next;
    int type;
    char filename[260];
    int size;
    unsigned char unknown11c[4];
    int start;
};

struct STREAMHEADERtag {
    int magic;
    unsigned char mutex[44];
    unsigned char unknown30[20];
    int state;
    unsigned char unknown48[4];
    int priority;
    unsigned char unknown50[16];
    int total;
    int buffered;
    unsigned char unknown68[4];
    REQUESTVIEW* current;
    unsigned char unknown70[8];
    char filename[256];
    int handle;
    int position;
    int fileOp;
    int requested;
};

extern "C" {
int FILESYS_completeop(int);
int FILESYS_open(const char*, int, int, void*);
void FILESYS_callbackop(int, void (*)(int, int, void*));
int FILESYS_close(int, int, void*);
int strcmp(const char*, const char*);
char* strcpy(char*, const char*);
void MUTEX_lock(void*);
void MUTEX_unlock(void*);
}

// File-local in the original (defined elsewhere in the file).
int parsechunks(STREAMHEADERtag*);
void restartstream(STREAMHEADERtag*, int);
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
    header->buffered += count;
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
        request->start = header->total;
        request->state = 2;
    }
    MUTEX_unlock(header->mutex);
    if (idle)
        return;
    header->buffered = header->total;
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
