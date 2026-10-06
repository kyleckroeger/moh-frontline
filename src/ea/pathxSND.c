// PATHX (streamed path playback): the last two functions of the file, the
// sync task that runs the installed service and the fade helper. The track
// record view is inferred from offsets; the service callback's parameters
// are not established (the task passes none that the code shows).
struct PathxTrackView {
    int stream;
    unsigned char unknown04[4];
    int volume;
    unsigned char unknown0c[2];
    char isSound;
    unsigned char unknown0f[85];
    int* sound;
};

extern PathxTrackView* pathxinfo[];
extern void (*pathService)(void);

extern "C" void SNDSYS_entercritical(void);
extern "C" void SNDSYS_leavecritical(void);
extern "C" int SNDautovol(int handle, int time, int volume);
extern "C" int SNDSTRM_autovol(int stream, int time, int volume);

int PATHX_isynctask(int, int) {
    if (pathService)
        pathService();
    return 0;
}

int PATHX_ifade(int track, int volume, int time, char immediate) {
    int result = 0;
    PathxTrackView* info = pathxinfo[track];

    if (!immediate) {
        SNDSYS_entercritical();
        info->volume = volume;
        if (info->isSound)
            result = SNDautovol(info->sound[1], time / 10, volume);
        else
            result = SNDSTRM_autovol(info->stream, time, volume);
        SNDSYS_leavecritical();
    }
    return result;
}
