// Fragment: selects the shell library table. The original file also holds the
// library's strings, which are not reconstructed here. Types are unknown.
extern void* LibraryInfo;
extern char ShellLibraryInfo[];

void ShellIsLibrary() {
    LibraryInfo = ShellLibraryInfo;
}
