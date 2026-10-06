// Fragment: selects the pause screen table. The original file also holds the
// screen-name strings, which are not reconstructed here. Types are unknown.
extern void* ScreenInfo;
extern int PauseScreenInfo[2];

void PauseIsScreen() {
    ScreenInfo = PauseScreenInfo;
}
