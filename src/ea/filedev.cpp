// The file device: reads are queued to the read thread, writes and size
// queries go through the MSL FILE functions for host files (handle type 3),
// and the cached size is used for disc files (type 2). HANDLE_def is named by
// the mangled symbols; its members and the read request record are inferred
// views.
extern "C" {
int fseek(void*, long, int);
long ftell(void*);
unsigned long fwrite(const void*, unsigned long, unsigned long, void*);
int OSSendMessage(void*, void*, int);
}

struct HANDLE_def {
    int type;
    char field04[260];
    void* file;
    char field10C[48];
    int size;
};

struct READREQUEST {
    char field00[60];
    HANDLE_def* handle;
    void* buffer;
    int offset;
    int size;
};

extern char ReadFileThreadMsgQ[];
// File-local in the original; declared without static here because this
// fragment does not define it (the manifest lists it as a local external).
extern READREQUEST gCurRead;

// initfiledev, the open/close functions and the asynchronous DVD read path
// come first in the original file and are not reconstructed, so this unit
// starts at readfile.

int readfile(HANDLE_def* handle, void* buffer, int offset, int size) {
    gCurRead.offset = offset;
    gCurRead.handle = handle;
    gCurRead.buffer = buffer;
    gCurRead.size = size;
    OSSendMessage(ReadFileThreadMsgQ, 0, 1);
    return 1;
}

int writefile(HANDLE_def* handle, void* buffer, int offset, int size) {
    if (handle->type == 3) {
        fseek(handle->file, offset, 0);
        fwrite(buffer, size, 1, handle->file);
        return 1;
    }
    return -1;
}

int getfilesize(HANDLE_def* handle) {
    if (handle->type == 3) {
        long position = ftell(handle->file);
        fseek(handle->file, 0, 2);
        long size = ftell(handle->file);
        fseek(handle->file, position, 0);
        return size;
    }
    if (handle->type == 2)
        return handle->size;
    return 0;
}

void stopreadfile(HANDLE_def*) {
}

int deletefile(const char*) {
    return -1;
}
