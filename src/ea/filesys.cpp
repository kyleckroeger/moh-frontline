// EA's file system, the functions at the start of the file: default memory
// callbacks that abort with a message, initialisation (installing the default
// callbacks when none are set, allocating the device's memory through the
// callback, or taking caller-supplied memory), the memory overhead for a
// number of files and handles, and shutdown. The function and global names
// come from the symbols; the options and device layouts are inferred views,
// and the parameters' meanings (handles, buffer size, files) are inferred
// from the defaults and the overhead formula.
struct OSMutex {
    unsigned char unknown00[24];
};

extern "C" {
void OSInitMutex(OSMutex*);
void MEM_fill(void*, int, int);
void REAL_abortmessage(const char*, ...);
int FILESYS_overhead(int, int, int);
int FILESYS_initadr(int, int, int, void*);
}

void initfiledev();
void killfiledev();

struct FILESYSOPTSVIEW {
    int unknown00;
    void* (*malloc)(const char*, int, int);
    int (*mfree)(void*);
};

struct FILEDEVICEVIEW {
    int files;
    int handles;
    int unknown08;
    unsigned char unknown0c[12];
    char* memory;
    char* handleMemory;
    unsigned char unknown20[8];
};

extern FILESYSOPTSVIEW gFileSysOpts;

static OSMutex FileMutex;
static FILEDEVICEVIEW gFileDevice;

static void* DefaultFILE_malloc(const char*, int, int) {
    REAL_abortmessage("DefaultFILE_malloc - must set memory callbacks with FILESYS_setopts.\n");
    return 0;
}

static int DefaultFILE_mfree(void*) {
    REAL_abortmessage("DefaultFILE_mfree - must set memory callbacks with FILESYS_setopts.\n");
    return 0;
}

extern "C" int FILESYS_init(int handles, int bufferSize, int files) {
    if (!gFileSysOpts.malloc) {
        gFileSysOpts.malloc = DefaultFILE_malloc;
        gFileSysOpts.mfree = DefaultFILE_mfree;
    }
    if (gFileDevice.files == 0) {
        void* memory = gFileSysOpts.malloc("File Sys", FILESYS_overhead(handles, bufferSize, files), 0);
        return FILESYS_initadr(handles, bufferSize, files, memory);
    }
    return 0;
}

extern "C" int FILESYS_initadr(int handles, int bufferSize, int files, void* memory) {
    if (!handles)
        handles = 24;
    if (!bufferSize)
        bufferSize = 2048;
    if (!files)
        files = 10;
    if (!gFileSysOpts.malloc) {
        gFileSysOpts.malloc = DefaultFILE_malloc;
        gFileSysOpts.mfree = DefaultFILE_mfree;
    }
    if (gFileDevice.files == 0) {
        gFileDevice.files = files;
        gFileDevice.handles = handles;
        gFileDevice.unknown08 = 255;
        gFileDevice.memory = (char*)memory;
        OSInitMutex(&FileMutex);
        MEM_fill(gFileDevice.memory, 0, FILESYS_overhead(handles, bufferSize, files));
        gFileDevice.handleMemory = gFileDevice.memory + gFileDevice.files * 48;
        initfiledev();
        return 1;
    }
    return 0;
}

extern "C" int FILESYS_overhead(int handles, int, int files) {
    if (!handles)
        handles = 24;
    if (!files)
        files = 10;
    return files * 48 + handles * 324;
}

extern "C" void FILESYS_restore() {
    if (gFileDevice.files) {
        killfiledev();
        gFileSysOpts.mfree(gFileDevice.memory);
        gFileDevice.files = 0;
    }
}
