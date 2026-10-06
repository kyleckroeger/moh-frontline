// ttDoVTimerMsg: the vertical-blank timer tick; counts and calls up to eight
// registered subscribers.
extern "C" {
extern volatile int vblticks;
extern void (*vbltmrsub[8])(void);
}

void ttDoVTimerMsg() {
    int i;

    vblticks++;
    for (i = 0; i < 8; i++) {
        if (vbltmrsub[i])
            vbltmrsub[i]();
    }
}
