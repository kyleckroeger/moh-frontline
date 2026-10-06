// A fragment of toilet.cpp (0x8001d978): TLT_UnLoadShellBriefingLogBigFile,
// which frees the shell briefing-log big file and clears its pointer and size.
// The file name is this project's; the original record is toilet.cpp and the
// functions around it are not reconstructed. The file's globals are extern.
// The level-file layer, the functions from TLT_CloseFile to
// TLT_UnLoadShellBriefingLogBigFile: closing a file frees it unless it lies
// inside the level or shell-briefing big file, the level-contents table is
// searched for the next resource of a type (optionally with a value in its
// data), and the shell-briefing big file is unloaded. The function, type and
// global names come from the symbols; the 12-byte contents entry and the
// resource data view (a count, then 8-byte entries) are inferred. The file's
// small globals are defined here in their original order. TLT_FindNextResource-
// ByTypeAndVal ignores its start argument and searches from the beginning.
extern "C" void MEM_free(void*);

enum TLTResourceID {};

struct TLTResourceDataView {
    int count;
    struct {
        int value;
        int unknown04;
    } entries[1];
};

struct LevelFileContentsStruct_ {
    TLTResourceID type;
    int unknown04;
    TLTResourceDataView* data;
};

extern char* g_pLevelBigFile;
extern char* g_pShellBriefingLogBigFile;
extern int g_LevelBigFileSize;
extern int g_ShellBriefingLogBigFileSize;
extern void* g_pCompartmentBigFileHeader;
extern void* g_CompartmentBigFile;
extern LevelFileContentsStruct_* g_pLevelFileContentsArray;
extern int* g_pNumLevelFileContents;
extern LevelFileContentsStruct_* g_pLevelFileContentsEndMarker;

void TLT_UnLoadShellBriefingLogBigFile() {
    if (g_pShellBriefingLogBigFile) {
        MEM_free(g_pShellBriefingLogBigFile);
        g_pShellBriefingLogBigFile = 0;
        g_ShellBriefingLogBigFileSize = 0;
    }
}

LevelFileContentsStruct_* TLT_FindNextResourceByType(TLTResourceID type, LevelFileContentsStruct_* previous);

LevelFileContentsStruct_* TLT_FindNextResourceByTypeAndVal(TLTResourceID type, int index, int value, LevelFileContentsStruct_*);

void TLT_CloseFile(void* file);


