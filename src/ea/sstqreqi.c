// SNDSTRM_queuerequestid: queue stream data by request id.
int SNDSTRMI_queue(int, int, char*, int, int);

extern "C" int SNDSTRM_queuerequestid(int stream, int flags, int requestid) {
    return SNDSTRMI_queue(stream, flags, 0, requestid, 2);
}
