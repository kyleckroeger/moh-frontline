// SNDSTRM_pitchmult: store a stream's pitch multiplier and apply it to its voice. The
// record's layout is not known; it is accessed through inferred offsets.
char* SNDSTRMI_getstreamptr(int);
extern "C" int SNDpitchmult(int, int);

extern "C" int SNDSTRM_pitchmult(int stream, int multiplier) {
    char* record = SNDSTRMI_getstreamptr(stream);

    if (!record)
        return -8;
    *(short*)(record + 208) = multiplier;
    SNDpitchmult(*(int*)(record + 4), multiplier);
    return 0;
}
