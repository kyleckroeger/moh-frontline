// REAL timer queries.
extern "C" {
extern int ticks;
extern int TIMERhz;

int TIMER_gettick(void) {
    return ticks;
}

int TIMER_getfrequency(void) {
    return TIMERhz;
}
}
