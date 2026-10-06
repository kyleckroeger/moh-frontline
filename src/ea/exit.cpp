// REAL exit handlers: up to 64 functions, run in reverse order of slot on
// restore or exit.
extern "C" {
void exit(int);

static void (*exitfunctions[64])(void) = { 0 };

void REAL_restore(void) {
    for (int i = 63; i >= 0; i--) {
        if (exitfunctions[i])
            exitfunctions[i]();
        exitfunctions[i] = 0;
    }
}

void REAL_exit(void) {
    REAL_restore();
    exit(0);
}

void REAL_addexit(void (*function)(void)) {
    int i;
    for (i = 0; i < 64; i++) {
        if (exitfunctions[i] == function)
            return;
    }
    for (i = 0; i < 64; i++) {
        if (exitfunctions[i] == 0) {
            exitfunctions[i] = function;
            return;
        }
    }
}

void REAL_removeexit(void (*function)(void)) {
    for (int i = 0; i < 64; i++) {
        if (exitfunctions[i] == function) {
            exitfunctions[i] = 0;
            return;
        }
    }
}
}
