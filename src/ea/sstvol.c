// SNDSTRM_vol: store a stream's volume and apply it to its voice. The
// record's layout is not known; it is accessed through inferred offsets.
char* SNDSTRMI_getstreamptr(int);
extern "C" int SNDvol(int, int);

extern "C" int SNDSTRM_vol(int stream, int volume) {
    char* record = SNDSTRMI_getstreamptr(stream);

    if (!record)
        return -8;
    *(char*)(record + 196) = volume;
    SNDvol(*(int*)(record + 4), volume);
    return 0;
}
