// SNDSTRM_timemult: store a stream's time multiplier and apply it to its voice. The
// record's layout is not known; it is accessed through inferred offsets.
char* SNDSTRMI_getstreamptr(int);
extern "C" int SNDCTRL_timemult(int, int);

extern "C" int SNDSTRM_timemult(int stream, int multiplier) {
    char* record = SNDSTRMI_getstreamptr(stream);

    if (!record)
        return -8;
    *(short*)(record + 210) = multiplier;
    SNDCTRL_timemult(*(int*)(record + 4), multiplier);
    return 0;
}
