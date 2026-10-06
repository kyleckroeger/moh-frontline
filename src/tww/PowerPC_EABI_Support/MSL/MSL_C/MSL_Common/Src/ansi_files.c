#include "ansi_files.h"
#include "file_io.h"
#include "critical_regions.h"

static unsigned char stdin_buff[0x100];

static unsigned char stdout_buff[0x100];

static unsigned char stderr_buff[0x100];

extern files __files = {
    {
        0,
        0,
        1,
        1,
        2,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        {0, 0},
        {0, 0},
        0,
        stdin_buff,
        0x00000100,
        stdin_buff,
        0,
        0,
        0,
        0,
        NULL,
        __read_console,
        __write_console,
        __close_console,
        NULL,
        &__files._stdout,
    },
    {
        1,
        0,
        2,
        1,
        2,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        {0, 0},
        {0, 0},
        0,
        stdout_buff,
        0x00000100,
        stdout_buff,
        0,
        0,
        0,
        0,
        NULL,
        __read_console,
        __write_console,
        __close_console,
        NULL,
        &__files._stderr,
    },
    {
        2,
        0,
        2,
        0,
        2,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        {0, 0},
        {0, 0},
        0,
        stderr_buff,
        0x00000100,
        stderr_buff,
        0,
        0,
        0,
        0,
        NULL,
        __read_console,
        __write_console,
        __close_console,
        NULL,
        &__files.empty,
    },
    {
        0,      0, 0,    0,          0,    0, 0, 0, 0, 0,    0,    0,    0,    0,    {0, 0},
        {0, 0}, 0, NULL, 0x00000000, NULL, 0, 0, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL,
    },
};

int __position_file(__file_handle, fpos_t*, int, __idle_proc);
int __read_file(__file_handle, unsigned char*, size_t*, __idle_proc);
int __write_file(__file_handle, unsigned char*, size_t*, __idle_proc);
int __close_file(__file_handle);
int setvbuf(FILE*, char*, int, size_t);
void* malloc(size_t);
void* memset(void*, int, size_t);

// Frontline also links __find_unopened_file, __init_file and
// __flush_line_buffered_output_files (not in the upstream file), reconstructed
// from the disassembly. -inline deferred emits functions in reverse source
// order, so these two precede __close_all and the third follows __flush_all.
FILE* __find_unopened_file(void) {
    FILE* file = __files._stderr.next_file;
    FILE* last_file;

    while (file != NULL) {
        if (file->file_mode.file_kind == __closed_file)
            return file;

        last_file = file;
        file = file->next_file;
    }

    file = (FILE*)malloc(sizeof(FILE));
    if (file != NULL) {
        memset(file, 0, sizeof(FILE));
        file->is_dynamically_allocated = 1;
        last_file->next_file = file;
        return file;
    }

    return NULL;
}

void __init_file(FILE* file, file_modes mode, char* buff, size_t size) {
    file->handle = 0;
    file->file_mode = mode;
    file->file_state.io_state = __neutral;
    file->file_state.free_buffer = 0;
    file->file_state.eof = 0;
    file->file_state.error = 0;
    file->position = 0;

    if (size)
        setvbuf(file, buff, _IOFBF, size);
    else
        setvbuf(file, NULL, _IONBF, 0);

    file->buffer_ptr = file->buffer;
    file->buffer_length = 0;

    if (file->file_mode.file_kind == __disk_file) {
        file->position_fn = __position_file;
        file->read_fn = __read_file;
        file->write_fn = __write_file;
        file->close_fn = __close_file;
    }

    file->idle_fn = NULL;
}

void __close_all(void) {
    FILE* file = &__files._stdin;
    FILE* last_file;

    while (file != NULL) {
        if (file->file_mode.file_kind != __closed_file) {
            fclose(file);
        }

        last_file = file;
        file = file->next_file;

        if (last_file->is_dynamically_allocated) {
            free(last_file);
        } else {
            last_file->file_mode.file_kind = __unavailable_file;
            if (file != NULL && file->is_dynamically_allocated) {
                last_file->next_file = NULL;
            }
        }
    }
}

unsigned int __flush_all(void) {
    unsigned int ret = 0;
    FILE* file = &__files._stdin;

    while (file) {
        if (file->file_mode.file_kind != __closed_file && fflush(file)) {
            ret = -1;
        }
        file = file->next_file;
    }

    return ret;
}

int __flush_line_buffered_output_files(void) {
    int result = 0;
    FILE* file = &__files._stdin;

    while (file != NULL) {
        if (file->file_mode.file_kind != __closed_file && (file->file_mode.buffer_mode & _IOLBF) &&
            file->file_state.io_state == __writing) {
            if (fflush(file))
                result = -1;
        }
        file = file->next_file;
    }

    return result;
}
