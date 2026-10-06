// The game's file layer over the Dolphin DVD library, the functions at the
// start of the file: seeking and writing are not supported (they fail), and
// closing a file closes its DVD handle, frees its slot and counts it out.
// The function names and ESeekOrigin come from the mangled symbols and the
// statics from their symbols; the open-file slot layout (an in-use flag and
// the DVD file information, 64 bytes) is inferred. File_Open follows and is
// not part of the unit.
struct DVDFileInfo {
    unsigned char unknown00[60];
};

extern "C" int DVDClose(DVDFileInfo*);

enum ESeekOrigin {};

struct OpenFileView {
    bool used;
    unsigned char unknown01[3];
    DVDFileInfo info;
};

static bool s_bInitialized;
static OpenFileView g_openFiles[32];
static int g_iNumOpenFiles;

int File_Seek(int, int, ESeekOrigin) {
    return -1;
}

int File_Write(int, void*, int) {
    return -1;
}

int File_Close(int file) {
    OpenFileView* open = &g_openFiles[file];
    DVDClose(&open->info);
    open->used = false;
    g_iNumOpenFiles--;
    return 0;
}
