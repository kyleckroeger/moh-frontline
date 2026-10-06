// The game's file layer over the Dolphin DVD library: seeking and writing
// are not supported (they fail); closing a file closes its DVD handle, frees
// its slot and counts it out; opening initialises the DVD library and clears
// the slots while s_bInitialized is unset (the target never sets it), then
// opens the file in the first free slot. The function names, ESeekOrigin and
// EOpenMode come from the mangled symbols and the statics from their symbols;
// the open-file slot layout (an in-use flag and the DVD file information, 64
// bytes) is inferred.
struct DVDFileInfo {
    unsigned char unknown00[60];
};

extern "C" {
void DVDInit();
int DVDOpen(const char*, DVDFileInfo*);
int DVDClose(DVDFileInfo*);
}

enum ESeekOrigin {};
enum EOpenMode {};

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

int File_Open(const char* name, EOpenMode) {
    OpenFileView* open;
    int slot;
    int i;
    if (!s_bInitialized) {
        DVDInit();
        for (i = 0; i < 32; i++)
            g_openFiles[i].used = false;
    }
    slot = -1;
    for (i = 0; i < 32; i++) {
        if (!g_openFiles[i].used) {
            slot = i;
            break;
        }
    }
    open = &g_openFiles[slot];
    if (!DVDOpen(name, &open->info))
        return -1;
    open->used = true;
    g_iNumOpenFiles++;
    return slot;
}
