// PATHX_readyfornewrequest and PATHX_getvolume (pathxSND.c, 0x800e93ac):
// refresh the track states when a request is pending, then report whether
// the track can take a new request, or its volume. The track record view is
// inferred (see pathxSND.c).
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

extern "C" int PATHX_statusall(int);

extern "C" int PATHX_readyfornewrequest(int track) {
    if (pathxinfo[track] && pathxinfo[track]->pending)
        PATHX_statusall(0);
    return pathxinfo[track]->ready;
}

extern "C" int PATHX_getvolume(int track) {
    if (pathxinfo[track] && pathxinfo[track]->pending)
        PATHX_statusall(0);
    return pathxinfo[track]->currentVolume;
}
