// SNDSTRM_setgreedylevel: pass a stream's greedy level to its file stream.
// The stream record's layout is not known; it is read through inferred
// offsets.
char* SNDSTRMI_getstreamptr(int);
extern "C" void STREAM_setgreedylevel(int, int);

extern "C" int SNDSTRM_setgreedylevel(int stream, int level) {
    char* record = SNDSTRMI_getstreamptr(stream);

    if (!record)
        return -8;
    STREAM_setgreedylevel(*(int*)record, level);
    return 0;
}
