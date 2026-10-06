// MEM_copy: forward copy using the widest access the relative alignment of
// source and destination allows (bytes, halfwords or words), after aligning
// the source.
extern "C" void MEM_copy(void* dest, const void* source, int size) {
    char* d = (char*)dest;
    const char* s = (const char*)source;
    int diff = s - d;

    if (size == 0 || diff == 0)
        return;
    if (diff & 1) {
        while (size >= 4) {
            int c0 = ((unsigned char*)s)[0];
            int c1 = ((unsigned char*)s)[1];
            int c2 = ((unsigned char*)s)[2];
            int c3 = ((unsigned char*)s)[3];

            d[0] = c0;
            d[1] = c1;
            d[2] = c2;
            d[3] = c3;
            d += 4;
            s += 4;
            size -= 4;
        }
        for (; size > 0; size--)
            *d++ = *s++;
    } else if (diff & 2) {
        if (((int)s & 1) && size >= 1) {
            *d = *s;
            s++;
            size--;
            d++;
        }
        while (size >= 8) {
            short h0 = ((short*)s)[0];
            short h1 = ((short*)s)[1];
            short h2 = ((short*)s)[2];
            short h3 = ((short*)s)[3];

            ((short*)d)[0] = h0;
            ((short*)d)[1] = h1;
            ((short*)d)[2] = h2;
            ((short*)d)[3] = h3;
            d += 8;
            s += 8;
            size -= 8;
        }
        while (size >= 2) {
            *(short*)d = *(short*)s;
            d += 2;
            s += 2;
            size -= 2;
        }
        if (size >= 1)
            *d = *s;
    } else {
        if (((int)s & 1) && size >= 1) {
            *d = *s;
            s++;
            size--;
            d++;
        }
        if (((int)s & 2) && size >= 2) {
            *(short*)d = *(short*)s;
            d += 2;
            s += 2;
            size -= 2;
        }
        while (size >= 16) {
            int w0 = ((int*)s)[0];
            int w1 = ((int*)s)[1];
            int w2 = ((int*)s)[2];
            int w3 = ((int*)s)[3];

            ((int*)d)[0] = w0;
            ((int*)d)[1] = w1;
            ((int*)d)[2] = w2;
            ((int*)d)[3] = w3;
            d += 16;
            s += 16;
            size -= 16;
        }
        while (size >= 4) {
            *(int*)d = *(int*)s;
            d += 4;
            s += 4;
            size -= 4;
        }
        if (size >= 2) {
            *(short*)d = *(short*)s;
            d += 2;
            s += 2;
            size -= 2;
        }
        if (size >= 1)
            *d = *s;
    }
}
