// SNDSTRM_status: a stream's state, current request and buffered time (ms,
// from the packet player or, when that is empty, the file stream). Records are
// accessed through inferred offsets.
char* SNDSTRMI_getstreamptr(int);
char* SNDSTRMI_getrequestptr(int);
extern "C" {
unsigned int SNDPKTPLAY_framesoutstanding(int);
unsigned int STREAM_gettable(int);
}

extern "C" int SNDSTRM_status(int stream, int* status) {
    char* record;

    status[2] = 0;
    status[1] = 0;
    status[0] = 0;
    record = SNDSTRMI_getstreamptr(stream);
    if (!record)
        return -8;
    status[0] = *(int*)(record + 252);
    if (status[0]) {
        status[1] = *(int*)(*(char**)(record + 244) + 12);
        if (*(unsigned short*)(record + 20)) {
            status[2] = SNDPKTPLAY_framesoutstanding(*(int*)(record + 8)) * 1000 / *(unsigned short*)(record + 20);
            if (status[2] == 0) {
                char* request = SNDSTRMI_getrequestptr(status[1]);

                if (*(unsigned int*)(request + 16)) {
                    unsigned int available = STREAM_gettable(*(int*)record);

                    if (available > 4000000)
                        available = 4000000;
                    status[2] = available * 1000 / *(unsigned int*)(request + 16);
                }
            }
        }
    }
    return 0;
}
