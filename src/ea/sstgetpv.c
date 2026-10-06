// SNDSTRM_getprogvol: a stream's program volume, from its voice or else the
// value stored in the stream record. The record's layout is not known; it is
// read through inferred offsets.
char* SNDSTRMI_getstreamptr(int);
extern "C" int SNDCTRL_getprogvol(int);

extern "C" int SNDSTRM_getprogvol(int stream) {
    char* record = SNDSTRMI_getstreamptr(stream);
    int volume;

    if (!record)
        return -8;
    volume = SNDCTRL_getprogvol(*(int*)(record + 4));
    if (volume < 0)
        volume = *(signed char*)(record + 196);
    return volume;
}
