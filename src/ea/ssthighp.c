// SNDSTRM_highpass: store a stream's high-pass setting and apply it to its
// voice. The record's layout is not known; it is accessed through inferred
// offsets.
char* SNDSTRMI_getstreamptr(int);
extern "C" int SNDCTRL_highpass(int, int);

extern "C" int SNDSTRM_highpass(int stream, int frequency) {
    char* record = SNDSTRMI_getstreamptr(stream);

    if (!record)
        return -8;
    *(short*)(record + 218) = frequency;
    SNDCTRL_highpass(*(int*)(record + 4), frequency);
    return 0;
}
