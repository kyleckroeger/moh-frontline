// Memory needed by a stream (40 bytes per channel plus 272), its packet
// player, and (non-tap streams) its file stream.
extern "C" {
int SNDPKTPLAY_overhead(int);
int STREAM_overhead(int, int, int);

int SNDSTRM_overheadtap(int channels, int buffers) {
    int size = channels * 40 + 272;

    size += SNDPKTPLAY_overhead(buffers);
    return size;
}

int SNDSTRM_overhead(int channels, int buffers) {
    int size = channels * 40 + 272;

    size += SNDPKTPLAY_overhead(buffers);
    return size + STREAM_overhead(channels + 2, 1, 1);
}
}
