// A fragment of debug_prof.cpp (0x800f4120); the file name is this project's. Debug profiling timers: starting, pausing, resuming and stopping a timer
// accumulate OS ticks into it (subtracting the tick at start or resume and
// adding it at pause or stop), count starts, track how many are running, and
// on stop carry whole billions of ticks into a second counter. The function
// and array names come from the symbols; the 52-byte timer layout (name and
// links before the counters) is inferred from offsets.
extern "C" unsigned long OSGetTick(void);

struct DebugProfTimerView {
    unsigned char unknown00[36];
    int starts;
    unsigned long ticks;
    int billions;
    int running;
};

extern DebugProfTimerView g_pDebugProfTimers[];

void DebugProfTimerStop(int index) {
    DebugProfTimerView* timer = &g_pDebugProfTimers[index];
    timer->running--;
    timer->ticks += OSGetTick();
    if (timer->ticks >= 1000000000) {
        timer->billions++;
        timer->ticks -= 1000000000;
    }
}

void DebugProfTimerResume(int index) {
    DebugProfTimerView* timer = &g_pDebugProfTimers[index];
    timer->running++;
    timer->ticks -= OSGetTick();
}

void DebugProfTimerPause(int index) {
    DebugProfTimerView* timer = &g_pDebugProfTimers[index];
    timer->running--;
    timer->ticks += OSGetTick();
}

void DebugProfTimerStart(int index) {
    DebugProfTimerView* timer = &g_pDebugProfTimers[index];
    timer->starts++;
    timer->running++;
    timer->ticks -= OSGetTick();
}
