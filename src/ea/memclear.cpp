// MEM_clear: zero a block of memory.
extern "C" {
void MEM_fill(void*, int, int);

void MEM_clear(void* address, int size) {
    MEM_fill(address, 0, size);
}
}
