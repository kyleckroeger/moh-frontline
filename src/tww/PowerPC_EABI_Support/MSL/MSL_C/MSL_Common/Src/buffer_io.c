#include "buffer_io.h"
#include "stdlib.h"

int fflush(FILE* file);

inline void __convert_from_newlines(unsigned char* buf, size_t* n) {}

inline void __convert_to_newlines(unsigned char* buf, size_t* n) {}

void __prep_buffer(FILE* file) {
    file->buffer_ptr = file->buffer;
    file->buffer_length = file->buffer_size;
    file->buffer_length -= file->position & file->buffer_alignment;
    file->buffer_position = file->position;
}

// Frontline also links __load_buffer and setvbuf (not in the upstream file),
// reconstructed from the disassembly. Source order is the reverse of the
// target's because -inline deferred emits functions in reverse order.
int __load_buffer(FILE* file, size_t* bytes_loaded, int alignment) {
    int ioresult;

    __prep_buffer(file);

    if (alignment == __dont_align_buffer)
        file->buffer_length = file->buffer_size;

    ioresult = (*file->read_fn)(file->handle, file->buffer, (size_t*)&file->buffer_length, file->idle_fn);

    if (ioresult == __io_EOF)
        file->buffer_length = 0;

    if (bytes_loaded)
        *bytes_loaded = file->buffer_length;

    if (ioresult)
        return ioresult;

    file->position += file->buffer_length;

    if (!file->file_mode.binary_io)
        __convert_to_newlines(file->buffer, (size_t*)&file->buffer_length);

    return __no_io_error;
}

int __flush_buffer(FILE* file, size_t* bytes_flushed) {
    size_t buffer_len;
    int ioresult;

    buffer_len = file->buffer_ptr - file->buffer;

    if (buffer_len) {
        file->buffer_length = buffer_len;

        if (!file->file_mode.binary_io)
            __convert_from_newlines(file->buffer, (size_t*)&file->buffer_length);

        ioresult = (*file->write_fn)(file->handle, file->buffer, (size_t*)&file->buffer_length,
                                     file->idle_fn);

        if (bytes_flushed)
            *bytes_flushed = file->buffer_length;

        if (ioresult)
            return (ioresult);

        file->position += file->buffer_length;
    }

    __prep_buffer(file);

    return 0;
}

int setvbuf(FILE* file, char* buff, int mode, size_t size) {
    int kind = file->file_mode.file_kind;

    if (mode == _IONBF)
        fflush(file);

    if (file->file_state.io_state != __neutral || kind == __closed_file)
        return -1;

    if (mode != _IONBF && mode != _IOLBF && mode != _IOFBF)
        return -1;

    if (file->buffer && file->file_state.free_buffer)
        free(file->buffer);

    file->file_mode.buffer_mode = mode;
    file->file_state.free_buffer = 0;
    file->buffer = (unsigned char*)&file->char_buffer;
    file->buffer_ptr = (unsigned char*)&file->char_buffer;
    file->buffer_size = 1;
    file->buffer_length = 0;
    file->buffer_alignment = 0;

    if (mode == _IONBF || size < 1) {
        *(file->buffer_ptr) = '\0';
        return 0;
    }

    if (!buff) {
        if (!(buff = (char*)malloc(size)))
            return -1;
        file->file_state.free_buffer = 1;
    }

    file->buffer = (unsigned char*)buff;
    file->buffer_ptr = file->buffer;
    file->buffer_size = size;
    file->buffer_alignment = 0;

    return 0;
}

