// Clears the current screen and library tables. Their types are unknown.
extern void* ScreenInfo;
extern void* LibraryInfo;

void NullifyScreenAndLibrary() {
    ScreenInfo = 0;
    LibraryInfo = 0;
}
