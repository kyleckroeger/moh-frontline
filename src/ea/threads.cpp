// REAL thread helpers: record the main thread, and the alarm handler that
// wakes a sleeping thread. The alarm and its signal are kept together so the
// handler can find the signal; that record and the OS structures are
// inferred views.
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
void SIGNAL_set(SIGNAL*);
}
void InitAlarm();

OSThread* g_thMain;

extern "C" void THREAD_init(void) {
    InitAlarm();
    g_thMain = OSGetCurrentThread();
}

static void AlarmHandler(OSAlarm* alarm, OSContext*) {
    SIGNAL_set(&((THREADALARM*)alarm)->signal);
}

// THREAD_yield and THREAD_iscurrent follow in the original file. THREAD_yield
// is drafted in scratch but not matched (the 64-bit tick product lands in
// other registers), so this unit covers only the functions before it.
