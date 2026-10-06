// Fragment: selects the shell screen table. The original file also holds the
// screen-name strings, which are not reconstructed here. Types are unknown.
extern void* ScreenInfo;
extern int ShellScreenInfo[2];

void ShellIsScreen() {
    ScreenInfo = ShellScreenInfo;
}
