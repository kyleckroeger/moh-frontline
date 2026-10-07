// The start of EA's file device (filedev.cpp): initfiledev starts the read
// thread (8 KB stack, priority 2) with its 32-message queue and killfiledev
// suspends it. openfile opens a host file through MSL (handle type 3) when
// the mode's bit 0 is clear or the name starts with "hd:\\", "hd:/" or "hd:"
// (compared case-insensitively), converting backslashes; otherwise it opens
// the disc file by path (type 2). MWIO_openfile accepts modes 0, 1 and 6 and
// aborts on the others. closefile closes either kind and reports success.
// The asynchronous DVD read path and the read thread after these are not
// part of this unit. HANDLE_def and DVDFileInfo are named by the mangled
// symbols; the handle's members, the read request and the prefix and path
// helpers are inferred. This file is compiled without automatic inlining
// (MWIO_openfile is called, not inlined) and with signed char.
namespace std {
struct _FILE;
typedef _FILE FILE;
}
using std::FILE;
struct OSThread {
    unsigned char unknown000[784];
};
struct OSMessageQueue {
    unsigned char unknown00[32];
};
typedef void* OSMessage;

struct DVDFileInfo {
    unsigned char commandBlock[48];
    unsigned long startAddr;
    unsigned long length;
    void* callback;
};

extern "C" {
int OSCreateThread(OSThread*, void* (*)(void*), void*, void*, unsigned long, long, unsigned short);
void OSInitMessageQueue(OSMessageQueue*, OSMessage*, long);
long OSResumeThread(OSThread*);
long OSSuspendThread(OSThread*);
int OSSendMessage(OSMessageQueue*, OSMessage, int);
FILE* fopen(const char*, const char*);
int fclose(FILE*);
int fseek(FILE*, long, int);
long ftell(FILE*);
unsigned long fwrite(const void*, unsigned long, unsigned long, FILE*);
void REAL_abortmessage(const char*, ...);
long DVDConvertPathToEntrynum(const char*);
int DVDFastOpen(long, DVDFileInfo*);
int DVDClose(DVDFileInfo*);
int DVDReadAsyncPrio(DVDFileInfo*, void*, long, long, void (*)(long, DVDFileInfo*), long);
long DVDGetDriveStatus();
void DCInvalidateRange(void*, unsigned long);
void MEM_copy(void*, const void*, int);
int OSReceiveMessage(OSMessageQueue*, OSMessage*, int);
unsigned long fread(void*, unsigned long, unsigned long, FILE*);
void THREAD_yield(int);
extern unsigned char __lower_map[];
}

static inline int tolower(int c) {
    return c == -1 ? -1 : (int)__lower_map[(unsigned char)c];
}

struct HANDLE_def {
    int type;
    char field04[260];
    union {
        FILE* file;
        DVDFileInfo dvd;
    };
};

/* inferred: the read in progress and its aligned bounce buffer */
struct READREQUEST {
    int state;
    char* dest;
    int offset;
    int size;
    char* destAlignedDown;
    char* destAlignedUp;
    char* destEnd;
    char* destEndAligned;
    int offsetAligned;
    int offsetAlignedUp;
    int offsetEnd;
    int offsetEndAligned;
    int misalign;
    int headLength;
    int queued;
    HANDLE_def* handle;
    void* buffer;
    int requestOffset;
    int requestSize;
    char field4c[20];
    char temp[32768];
};

static OSThread ReadFileThread;
static char ReadFileThreadStack[8192];
OSMessageQueue ReadFileThreadMsgQ;
OSMessage ReadFileThreadMsgData[32];
// File-local in the original; this fragment does not define it.
extern READREQUEST gCurRead;

void* ReadFileThreadFunc(void*);
void iFILE_CommandCompleteCallback(int);

void initfiledev() {
    OSCreateThread(&ReadFileThread, ReadFileThreadFunc, 0, ReadFileThreadStack + sizeof(ReadFileThreadStack),
                   sizeof(ReadFileThreadStack), 2, 1);
    OSInitMessageQueue(&ReadFileThreadMsgQ, ReadFileThreadMsgData, 32);
    OSResumeThread(&ReadFileThread);
}

void killfiledev() {
    OSSuspendThread(&ReadFileThread);
}

static int MWIO_openfile(const char* name, unsigned int mode, FILE** file) {
    switch (mode & 7) {
    default:
        REAL_abortmessage("openfile - ILLEGAL OPEN MODE (%d).\n", mode);
        *file = 0;
        return 0;
    case 0:
    case 1:
    case 6:
        *file = fopen(name, "a+");
        return *file != 0;
    }
}

static inline int prefixlength(const char* name, const char* prefix) {
    int j = 0;

    while (prefix[j]) {
        if (prefix[j] != tolower(name[j]))
            return 0;
        j++;
    }
    return j;
}

static inline void copypath(char* dst, const char* src) {
    int k;

    for (k = 0; src[k]; k++) {
        if (src[k] == '\\')
            dst[k] = '/';
        else
            dst[k] = src[k];
    }
    dst[k] = src[k];
}

int openfile(const char* name, unsigned int mode, HANDLE_def* handle) {
    int length = 0;
    const char* prefixes[3] = {"hd:\\", "hd:/", "hd:"};
    int host = (mode & 1) == 0;
    DVDFileInfo* info = &handle->dvd;
    char path[256];
    int result;
    int i;

    if (!host) {
        for (i = 0; i < 3; i++) {
            length = prefixlength(name, prefixes[i]);
            if (length) {
                host = 1;
                break;
            }
        }
    }
    if (host) {
        copypath(path, name + length);
        result = MWIO_openfile(path, mode, &handle->file);
        if (!result)
            fclose(handle->file);
        else
            handle->type = 3;
        return result;
    } else {
        long entry;
        int k;

        for (k = 0; name[k]; k++) {
            if (name[k] == '\\')
                path[k] = '/';
            else
                path[k] = name[k];
        }
        path[k] = name[k];
        entry = DVDConvertPathToEntrynum(path);
        if (entry == -1) {
            result = 0;
        } else {
            result = DVDFastOpen(entry, info);
            if (!result)
                DVDClose(&handle->dvd);
        }
        if (result)
            handle->type = 2;
        return result;
    }
}

int closefile(HANDLE_def* handle) {
    int result = 0;

    if (handle->type == 3)
        result = fclose(handle->file);
    else if (handle->type == 2)
        result = DVDClose(&handle->dvd);
    return result == 0;
}

