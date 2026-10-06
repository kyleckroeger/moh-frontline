// FONT_bsearch: binary search of a font's character table (entries of
// `stride` bytes, each starting with the 16-bit character code).
extern "C" void* FONT_bsearch(int ch, char* table, int count, int stride) {
    while (count != 0) {
        char* entry = table + (count >> 1) * stride;
        int diff = ch - *(unsigned short*)entry;

        if (diff == 0)
            return entry;
        if (diff > 0) {
            table = entry + stride;
            count--;
        }
        count >>= 1;
    }
    return 0;
}
