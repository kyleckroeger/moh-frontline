/* MSL bsearch.c, reconstructed from the disassembly. Element 0 is compared
 * first; the binary search then covers elements 1 to num - 1. */
typedef unsigned long size_t;
#define NULL 0

void* bsearch(const void* key, const void* base, size_t num, size_t size,
              int (*compare)(const void*, const void*)) {
    size_t l, r, m;
    int c;
    char* mp;

    if (!key || !base || !num || !size || !compare)
        return NULL;

    mp = (char*)base;
    c = compare(key, mp);

    if (c == 0)
        return mp;

    if (c < 0)
        return NULL;

    l = 1;
    r = num - 1;

    while (l <= r) {
        m = (l + r) / 2;
        mp = (char*)base + size * m;
        c = compare(key, mp);

        if (c == 0)
            return mp;

        if (c < 0)
            r = m - 1;
        else
            l = m + 1;
    }

    return NULL;
}
