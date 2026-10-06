/* MSL signal.c: raise. Reconstructed from the disassembly; the handler table
 * has six entries (signals 1-6), and signal 1 is SIGABRT. */
typedef void (*__signal_func_ptr)(int);

#define SIG_DFL ((__signal_func_ptr)0)
#define SIG_IGN ((__signal_func_ptr)1)
#define SIGABRT 1

void exit(int);

static __signal_func_ptr signal_funcs[6];

int raise(int sig) {
    __signal_func_ptr handler;

    if (sig < 1 || sig > 6)
        return -1;

    handler = signal_funcs[sig - 1];
    if (handler != SIG_IGN)
        signal_funcs[sig - 1] = SIG_DFL;

    if (handler == SIG_IGN || (handler == SIG_DFL && sig == SIGABRT))
        return 0;

    if (handler == SIG_DFL)
        exit(0);

    handler(sig);
    return 0;
}
