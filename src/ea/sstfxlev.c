// SNDSTRM_fxlevel: store a stream's effect level and apply it to its voice.
// The record's layout is not known; it is accessed through inferred offsets.
char* SNDSTRMI_getstreamptr(int);
extern "C" int SNDfxlevel(int, int, int);

extern "C" int SNDSTRM_fxlevel(int stream, int bus, int level) {
    char* record = SNDSTRMI_getstreamptr(stream);

    if (!record)
        return -8;
    *(char*)(record + 201) = level;
    SNDfxlevel(*(int*)(record + 4), bus, level);
    return 0;
}
