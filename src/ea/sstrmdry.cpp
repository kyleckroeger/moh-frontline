// SNDSTRM_drylevel: store a stream's dry level and apply it to its voice. The
// record's layout is not known; it is accessed through inferred offsets.
char* SNDSTRMI_getstreamptr(int);
extern "C" int SNDCTRL_drylevel(int, int);

extern "C" int SNDSTRM_drylevel(int stream, int level) {
    char* record = SNDSTRMI_getstreamptr(stream);

    if (!record)
        return -8;
    *(char*)(record + 200) = level;
    SNDCTRL_drylevel(*(int*)(record + 4), level);
    return 0;
}
