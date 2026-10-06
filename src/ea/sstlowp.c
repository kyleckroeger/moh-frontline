// SNDSTRM_lowpass: store a stream's low-pass setting and apply it to its voice. The
// record's layout is not known; it is accessed through inferred offsets.
char* SNDSTRMI_getstreamptr(int);
extern "C" int SNDCTRL_lowpass(int, int);

extern "C" int SNDSTRM_lowpass(int stream, int frequency) {
    char* record = SNDSTRMI_getstreamptr(stream);

    if (!record)
        return -8;
    *(short*)(record + 216) = frequency;
    SNDCTRL_lowpass(*(int*)(record + 4), frequency);
    return 0;
}
