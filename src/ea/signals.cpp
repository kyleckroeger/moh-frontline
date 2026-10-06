// REAL signals on an OS message queue with a single slot. The record's layout
// is inferred from offsets.
struct OSMessageQueue {
    int data[8];
};

struct SIGNAL {
    int field0;
    OSMessageQueue queue;
    void* message[1];
};

extern "C" {
void OSInitMessageQueue(OSMessageQueue*, void**, int);
int OSSendMessage(OSMessageQueue*, void*, int);
int OSReceiveMessage(OSMessageQueue*, void*, int);

int SIGNAL_create(SIGNAL* signal) {
    OSInitMessageQueue(&signal->queue, signal->message, 1);
    return 1;
}

void SIGNAL_set(SIGNAL* signal) {
    int message = 0;

    OSSendMessage(&signal->queue, &message, 0);
}

void SIGNAL_wait(SIGNAL* signal) {
    int message;

    OSReceiveMessage(&signal->queue, &message, 1);
}
}
