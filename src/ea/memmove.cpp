// MEM_move: copy that allows overlap (backwards when the destination starts
// inside the source).
extern "C" {
void MEM_copy(void*, const void*, int);

void MEM_move(void* dest, const void* source, int size) {
    if (dest <= source || dest >= (const char*)source + size) {
        MEM_copy(dest, source, size);
    } else {
        const char* s = (const char*)source + size;
        char* d = (char*)dest + size;

        while (size--)
            *--d = *--s;
    }
}
}
