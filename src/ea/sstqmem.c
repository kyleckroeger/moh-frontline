// SNDSTRM_queuemem: queue stream data that is already in memory.
int SNDSTRMI_queue(int, int, char*, int, int);

extern "C" int SNDSTRM_queuemem(int stream, int flags, char* buffer, int offset) {
    return SNDSTRMI_queue(stream, flags, buffer + offset, 0, 1);
}
