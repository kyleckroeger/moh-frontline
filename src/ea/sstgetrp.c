// SNDSTRMI_getrequestptr: find a queued request by id in its stream's request
// list (the stream index is the id's low byte). Records are accessed through
// inferred offsets.
char* SNDSTRMI_getstreamptr(int);

char* SNDSTRMI_getrequestptr(int id) {
    char* record;
    char* request;

    if (id < 0)
        return 0;
    record = SNDSTRMI_getstreamptr(id & 0xff);
    if (!record)
        return 0;
    for (request = *(char**)(record + 244); request; request = *(char**)request) {
        if (*(int*)(request + 12) == id)
            break;
    }
    return request;
}
