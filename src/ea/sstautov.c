// SNDSTRM_autovol: store a stream's volume and fade it on its voice (the
// time is given in tenths). The record's layout is not known; it is accessed
// through inferred offsets.
char* SNDSTRMI_getstreamptr(int);
extern "C" int SNDautovol(int, int, int);

extern "C" int SNDSTRM_autovol(int stream, int time, int volume) {
    char* record = SNDSTRMI_getstreamptr(stream);

    if (!record)
        return -8;
    *(char*)(record + 196) = volume;
    SNDautovol(*(int*)(record + 4), time / 10, volume);
    return 0;
}
