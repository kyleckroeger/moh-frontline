// The level-file layer's TLT_CloseFile: a file is freed unless it lies inside
// the level or shell-briefing big file (files loaded from a big file point
// into it). The function and global names come from the symbols; the globals
// are defined with the rest of the file. The resource searches after this
// function are drafted in scratch/lib/toilet_wip.cpp (a few lines off).
extern "C" void MEM_free(void*);

extern char* g_pLevelBigFile;
extern char* g_pShellBriefingLogBigFile;
extern int g_LevelBigFileSize;
extern int g_ShellBriefingLogBigFileSize;

void TLT_CloseFile(void* file) {
    if ((char*)file >= g_pLevelBigFile && (char*)file < g_pLevelBigFile + g_LevelBigFileSize)
        return;
    if ((char*)file >= g_pShellBriefingLogBigFile && (char*)file < g_pShellBriefingLogBigFile + g_ShellBriefingLogBigFileSize)
        return;
    MEM_free(file);
}
