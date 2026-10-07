// PATHX_modifyhold (pathxSND.c, 0x800e8a1c): passes a hold change to a
// streamed track's stream (sound-bank tracks are left alone). The track
// record view is inferred (see pathxSND.c).
struct PathxTrackView {
    int stream;
    unsigned char unknown04[10];
    signed char isSound;
    unsigned char unknown0f[61];
    int ready;
    unsigned char unknown50[4];
    int currentVolume;
    int pending;
    unsigned char unknown5c[8];
    int* sound;
};

extern PathxTrackView* pathxinfo[];

extern "C" int SNDSTRM_modifyhold(int, int);

extern "C" void PATHX_modifyhold(int track, int hold) {
    PathxTrackView* info = pathxinfo[track];

    if (!info->isSound)
        SNDSTRM_modifyhold(info->sound[1], hold);
}
