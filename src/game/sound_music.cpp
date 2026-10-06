// A fragment of sound.cpp (0x800ea528): the music wrappers over the path
// player. MUSIC_SendEvent sends the project's event (project id in the high
// byte) to the music handle, MUSIC_SetLatency and MUSIC_SetLevel clamp the
// latency (50 to 5000) and control level (0 to 127) before passing them on,
// MUSIC_Pause pauses or resumes every path when a music handle exists, and
// MUSIC_Stop stops the music handle. The file name is this project's; the
// original record is sound.cpp (sound.cpp holds the sound-stream wrappers)
// and MUSIC_Fade before these is not reconstructed. The functions and globals
// are named by their symbols; the result types and the path player
// prototypes as seen from this file are inferred.
extern "C" {
int PATH_event(unsigned int, short, int);
int PATH_latency(unsigned int, int);
int PATH_control(unsigned int, int);
void PATH_pause(unsigned int, signed char);
void PATH_stop(unsigned int);
}

extern int gMusicHandle;
extern int gMusicProjectID;

bool MUSIC_SendEvent(int event) {
    PATH_event(gMusicHandle, (gMusicProjectID << 8) | event, 1);
    return true;
}

bool MUSIC_SetLatency(int latency) {
    PATH_latency(gMusicHandle, latency < 50 ? 50 : (latency > 5000 ? 5000 : latency));
    return true;
}

bool MUSIC_SetLevel(int level) {
    PATH_control(gMusicHandle, level < 0 ? 0 : (level > 127 ? 127 : level));
    return true;
}

bool MUSIC_Pause(bool pause) {
    if (gMusicHandle < 0)
        return false;
    PATH_pause(0xFFFFFFFF, pause != 0);
    return pause;
}

void MUSIC_Stop() {
    if (gMusicHandle >= 0)
        PATH_stop(gMusicHandle);
}
