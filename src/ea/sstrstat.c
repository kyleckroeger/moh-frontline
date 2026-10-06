// SNDSTRM_requeststatus: a queued request's state (0 idle, 1 queued, 2
// playing, 3 unknown) and its positions in milliseconds. Records are accessed
// through inferred offsets; SINT64 is named by the mangled symbols.
struct SINT64 {
    unsigned int lo;
    unsigned int hi;
};

char* SNDSTRMI_getstreamptr(int);
char* SNDSTRMI_getrequestptr(int);
SINT64 iSNDmulu64(unsigned int, unsigned int);
unsigned int iSNDdivu64(SINT64, int);

extern "C" int SNDSTRM_requeststatus(int request, unsigned int* status) {
    char* record;
    char* entry;
    unsigned int rate;

    status[0] = 0;
    status[1] = 0;
    status[2] = 0;
    status[3] = 0;
    if (request < 0)
        return -8;
    record = SNDSTRMI_getstreamptr(request & 0xff);
    if (!record)
        return -8;
    entry = SNDSTRMI_getrequestptr(request);
    if (entry) {
        if (*(unsigned char*)(entry + 36) == 0) {
            status[0] = 0;
        } else {
            if (*(char**)(record + 244) == entry) {
                status[0] = 2;
                rate = *(unsigned short*)(record + 20);
            } else {
                status[0] = 1;
                rate = *(unsigned short*)(record + 24);
            }
            status[1] = iSNDdivu64(iSNDmulu64(*(unsigned int*)(entry + 20), 1000), rate);
            status[2] = iSNDdivu64(iSNDmulu64(*(unsigned int*)(entry + 24) - *(unsigned int*)(entry + 20), 1000), rate);
            status[3] = *(unsigned int*)(entry + 28) * 1000 / rate;
        }
    } else {
        status[0] = 3;
    }
    return 0;
}
