// SNDSTRM_3dpos: store a stream's 3D position (azimuth and distance) and
// apply it to its voice. The record's layout is not known; it is accessed
// through inferred offsets.
char* SNDSTRMI_getstreamptr(int);
extern "C" int SND3dpos(int, int, int);

extern "C" int SNDSTRM_3dpos(int stream, int azimuth, int distance) {
    char* record = SNDSTRMI_getstreamptr(stream);

    if (!record)
        return -8;
    *(short*)(record + 204) = azimuth;
    *(short*)(record + 206) = distance;
    SND3dpos(*(int*)(record + 4), azimuth, distance);
    return 0;
}
