// High-level file services: existence, size and whole-file loads (optionally
// returning the size), and BIG archive headers, each run as an atomic
// FILESYS operation. Allocation goes through gFileSysOpts. The request record
// and gFileSysOpts members are inferred from offsets and are not original.
struct FILEREQUEST {
    const char* name;
    int field4;
    int size;
    int sync;
    int memflags;
};

struct FILESYSOPTS {
    int field0;
    void* (*alloc)(const char*, int, int);
    void (*free)(void*);
};

extern "C" {
extern FILESYSOPTS gFileSysOpts;
bool FILESYS_opensync(const char*, int, int, int*);
int FILESYS_sizesync(int, int);
bool FILESYS_closesync(int, int);
int FILESYS_readsync(int, int, void*, int, int);
bool FILESYS_existssync(const char*, int);
int FILESYS_atomic(int (*)(int, void*), int, int, void*);
int BIG_typeofheader(void*);
int BIG_sizeofheader(void*);
void MEM_copy(void*, const void*, int);
}

static int sync_priority = 100;

extern "C" bool FILE_exists(const char* name) {
    return FILESYS_existssync(name, sync_priority);
}

static int filesizeatom(int priority, void* data) {
    FILEREQUEST* request = (FILEREQUEST*)data;
    int handle;

    if (FILESYS_opensync(request->name, 1, priority, &handle)) {
        int size = FILESYS_sizesync(handle, priority - 1);

        FILESYS_closesync(handle, priority - 1);
        return size;
    }
    return 0;
}

extern "C" int FILE_size(const char* name) {
    FILEREQUEST request;

    request.name = name;
    request.sync = 1;
    return FILESYS_atomic(filesizeatom, 0, sync_priority, &request);
}

extern "C" int FILE_sizez(const char* name) {
    FILEREQUEST request;

    request.name = name;
    request.sync = 0;
    return FILESYS_atomic(filesizeatom, 0, sync_priority, &request);
}

static int loadfileatom(int priority, void* data) {
    FILEREQUEST* request;
    int handle;
    void* buffer;
    int size;

    request = (FILEREQUEST*)data;

    if (FILESYS_opensync(request->name, 1, priority, &handle)) {
        size = FILESYS_sizesync(handle, priority - 1);
        buffer = gFileSysOpts.alloc(request->name, size, request->memflags);

        if (buffer) {
            FILESYS_readsync(handle, 0, buffer, size, priority - 1);
            FILESYS_closesync(handle, priority - 1);
            return (int)buffer;
        }
        FILESYS_closesync(handle, priority - 1);
        return 0;
    }
    return 0;
}

extern "C" void* FILE_loadz(const char* name, int memflags) {
    FILEREQUEST request;

    request.name = name;
    request.memflags = memflags;
    request.sync = 0;
    return (void*)FILESYS_atomic(loadfileatom, 0, sync_priority, &request);
}

static int loadfilesizeatom(int priority, void* data) {
    FILEREQUEST* request;
    int handle;
    void* buffer;
    int size;

    request = (FILEREQUEST*)data;

    if (FILESYS_opensync(request->name, 1, priority, &handle)) {
        size = FILESYS_sizesync(handle, priority - 1);
        buffer = gFileSysOpts.alloc(request->name, size, request->memflags);

        if (buffer) {
            FILESYS_readsync(handle, 0, buffer, size, priority - 1);
            FILESYS_closesync(handle, priority - 1);
            request->size = size;
            return (int)buffer;
        }
        FILESYS_closesync(handle, priority - 1);
        return 0;
    }
    return 0;
}

extern "C" void* FILE_loadsize(const char* name, int* size, int memflags) {
    FILEREQUEST request;
    void* buffer;

    request.name = name;
    request.memflags = memflags;
    request.sync = 1;
    buffer = (void*)FILESYS_atomic(loadfilesizeatom, 0, sync_priority, &request);
    if (size)
        *size = request.size;
    return buffer;
}

static int loadbigheaderatom(int priority, void* data) {
    FILEREQUEST* request = (FILEREQUEST*)data;
    int handle;

    if (FILESYS_opensync(request->name, 1, priority, &handle)) {
        char* header = (char*)gFileSysOpts.alloc(request->name, 2704, request->size);

        if (header) {
            int size;

            FILESYS_readsync(handle, 0, header, 2704, priority - 1);
            if (BIG_typeofheader(header) == 0) {
                gFileSysOpts.free(header);
                FILESYS_closesync(handle, priority - 1);
                return 0;
            }
            size = BIG_sizeofheader(header);
            if (size > 2704) {
                char* full = (char*)gFileSysOpts.alloc(request->name, size, request->size);

                if (full) {
                    MEM_copy(full, header, 2704);
                    gFileSysOpts.free(header);
                    header = full;
                    FILESYS_readsync(handle, 2704, full + 2704, size - 2704, priority - 1);
                } else {
                    gFileSysOpts.free(header);
                    FILESYS_closesync(handle, priority - 1);
                    return 0;
                }
            }
            FILESYS_closesync(handle, priority - 1);
            return (int)header;
        }
        FILESYS_closesync(handle, priority - 1);
        return 0;
    }
    return 0;
}

extern "C" void* FILE_loadbigheader(const char* name, int memflags) {
    FILEREQUEST request;

    request.name = name;
    request.size = memflags;
    request.sync = 1;
    return (void*)FILESYS_atomic(loadbigheaderatom, 0, sync_priority, &request);
}
