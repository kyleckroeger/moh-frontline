#include "abort_exit.h"
#include "stddef.h"
#include "NMWException.h"
#include "signal.h"

void _ExitProcess();

extern void (*_dtors[])(void);

static void (*__atexit_funcs[64])(void);

void (*__console_exit)(void);

void (*__stdio_exit)(void);

static int __atexit_curr_func;

int __aborting;

// Frontline also links abort (not in the upstream file): raise(SIGABRT),
// then the part of exit that still runs once __aborting is set. It is placed
// before exit because -inline deferred emits functions in reverse order.
void abort(void) {
    raise(1);
    __aborting = 1;

    while (__atexit_curr_func > 0)
        __atexit_funcs[--__atexit_curr_func]();

    if (__console_exit != NULL) {
        __console_exit();
        __console_exit = NULL;
    }

    _ExitProcess();
}

void exit(int status) {
    int i;
    void (**dtor)(void);

    if (!__aborting) {
        __destroy_global_chain();

        dtor = _dtors;
        while (*dtor != NULL) {
            (*dtor)();
            dtor++;
        }

        if (__stdio_exit != NULL) {
            __stdio_exit();
            __stdio_exit = NULL;
        }
    }

    while (__atexit_curr_func > 0)
        __atexit_funcs[--__atexit_curr_func]();

    if (__console_exit != NULL) {
        __console_exit();
        __console_exit = NULL;
    }

    _ExitProcess();
}
