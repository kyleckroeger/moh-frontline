// Fragment: selects the pause library table. The original file also holds the
// library's strings, which are not reconstructed here. Types are unknown.
extern void* LibraryInfo;
extern char PauseLibraryInfo[];

void PauseIsLibrary() {
    LibraryInfo = PauseLibraryInfo;
}
