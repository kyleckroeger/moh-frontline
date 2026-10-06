// A fragment of EA's threads.cpp (0x8015206c): THREAD_iscurrent, which compares
// the current thread with the given one (null meaning the main thread, -1 the
// current thread). The file name is this project's; the original record is
// threads.cpp (threads.cpp holds THREAD_init and the alarm handler) and the
// functions around it are not reconstructed. The file's globals are extern.
// REAL thread helpers: the main thread, a sleep built from an OS alarm that
// sets a signal, and a current-thread test (0 means the main thread, -1 the
// current one). The alarm and its signal are kept together so the handler
// can find the signal; that record and the OS structures are inferred views.
struct OSAlarm {
    int data[10];
};

struct OSContext;
struct OSThread;

struct OSMessageQueue {
    int data[8];
};

struct SIGNAL {
    int field0;
    OSMessageQueue queue;
    void* message[1];
};

struct THREADALARM {
    OSAlarm alarm;
    SIGNAL signal;
};

extern "C" {
OSThread* OSGetCurrentThread(void);
void OSYieldThread(void);
void OSCreateAlarm(OSAlarm*);
void OSSetAlarm(OSAlarm*, long long, void (*)(OSAlarm*, OSContext*));
void OSCancelAlarm(OSAlarm*);
int SIGNAL_create(SIGNAL*);
void SIGNAL_set(SIGNAL*);
void SIGNAL_wait(SIGNAL*);
}
void InitAlarm();

extern OSThread* g_thMain;

extern "C" void THREAD_init(void);

static void AlarmHandler(OSAlarm* alarm, OSContext*);

extern "C" void THREAD_yield(int milliseconds);

extern "C" bool THREAD_iscurrent(OSThread* thread) {
    OSThread* current = OSGetCurrentThread();

    return current == (thread == 0 ? g_thMain : thread == (OSThread*)-1 ? OSGetCurrentThread() : thread);
}


