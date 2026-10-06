// MEM_fill: store `value` over a block, aligning up to 32 bytes with byte,
// halfword and word stores, then in 32-byte blocks, then the tail. The value
// is stored as given (callers pass a replicated pattern).
extern "C" void MEM_fill(void* dest, int value, int size) {
    char* p = (char*)dest;

    if (size >= 1 && ((int)p & 1)) {
        *p = value;
        p += 1;
        size -= 1;
    }
    if (size >= 2 && ((int)p & 2)) {
        *(short*)p = value;
        p += 2;
        size -= 2;
    }
    if (size >= 4 && ((int)p & 4)) {
        *(int*)p = value;
        p += 4;
        size -= 4;
    }
    if (size >= 8 && ((int)p & 8)) {
        ((int*)p)[0] = value;
        ((int*)p)[1] = value;
        p += 8;
        size -= 8;
    }
    if (size >= 16 && ((int)p & 16)) {
        ((int*)p)[0] = value;
        ((int*)p)[1] = value;
        ((int*)p)[2] = value;
        ((int*)p)[3] = value;
        p += 16;
        size -= 16;
    }
    while (size >= 32) {
        ((int*)p)[0] = value;
        ((int*)p)[1] = value;
        ((int*)p)[2] = value;
        ((int*)p)[3] = value;
        ((int*)p)[4] = value;
        ((int*)p)[5] = value;
        ((int*)p)[6] = value;
        ((int*)p)[7] = value;
        p += 32;
        size -= 32;
    }
    if (size >= 16) {
        ((int*)p)[0] = value;
        ((int*)p)[1] = value;
        ((int*)p)[2] = value;
        ((int*)p)[3] = value;
        p += 16;
        size -= 16;
    }
    if (size >= 8) {
        ((int*)p)[0] = value;
        ((int*)p)[1] = value;
        p += 8;
        size -= 8;
    }
    if (size >= 4) {
        *(int*)p = value;
        p += 4;
        size -= 4;
    }
    if (size >= 2) {
        *(short*)p = value;
        p += 2;
        size -= 2;
    }
    if (size >= 1)
        *p = value;
}
