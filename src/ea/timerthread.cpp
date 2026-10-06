// The REAL timer thread: a message loop that runs the timer handlers until it
// is told to stop. OS structures are declared by size only.
struct OSMessageQueue {
    unsigned char unknown00[32];
};

struct OSThread {
    unsigned char unknown00[784];
};

typedef void* OSMessage;

// Messages: 0 runs the timers, 1 the vertical-retrace timers, 2 stops the
// thread. The enumerator names are not known.
enum TIMERMSG {};

extern "C" {
int OSCreateThread(OSThread*, void* (*)(void*), void*, void*, unsigned long, long, unsigned short);
void OSInitMessageQueue(OSMessageQueue*, OSMessage*, long);
long OSResumeThread(OSThread*);
int OSSendMessage(OSMessageQueue*, OSMessage, long);
int OSReceiveMessage(OSMessageQueue*, OSMessage*, long);
}

void ttDoTimerMsg();
void ttDoVTimerMsg();

OSMessageQueue TimerThreadMsgQ;
OSMessage TimerThreadMsgData[32];
OSThread TimerThread;
unsigned char TimerThreadStack[4096];
int TimesInited;

static void* TimerThreadFunc(void*);
void ttMsg(TIMERMSG);

void ttInit() {
    if (TimesInited == 0) {
        OSCreateThread(&TimerThread, TimerThreadFunc, 0, TimerThreadStack + sizeof(TimerThreadStack),
                       sizeof(TimerThreadStack), 4, 1);
        OSInitMessageQueue(&TimerThreadMsgQ, TimerThreadMsgData, 32);
        OSResumeThread(&TimerThread);
    }
    TimesInited++;
}

void ttKill() {
    if (--TimesInited == 0)
        ttMsg((TIMERMSG)2);
}

void ttMsg(TIMERMSG message) {
    OSSendMessage(&TimerThreadMsgQ, (OSMessage)message, 0);
}

static void* TimerThreadFunc(void*) {
    OSMessage message = 0;
    while ((int)message != 2) {
        OSReceiveMessage(&TimerThreadMsgQ, &message, 1);
        if ((int)message == 0)
            ttDoTimerMsg();
        else if ((int)message == 1)
            ttDoVTimerMsg();
    }
    return 0;
}
