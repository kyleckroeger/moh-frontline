#include "misc_io.h"
#include "abort_exit.h"
#include "ansi_files.h"

// Frontline also links clearerr (not in the upstream file). It precedes
// __stdio_atexit because -inline deferred emits functions in reverse order.
void clearerr(FILE* stream) {
    stream->file_state.eof = 0;
    stream->file_state.error = 0;
}

void __stdio_atexit(void) {
    __stdio_exit = __close_all;
}
