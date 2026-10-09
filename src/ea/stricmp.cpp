// EA's case-insensitive string compare: each character is lowered when it is
// an upper-case letter (the test sets a flag first), and the first
// difference, or 0 at the end of the first string, is returned. The flag
// form of the test and the inline helper are inferred from the code; the
// helper's name is this project's.
static inline char LowerChar(char c) {
    bool upper = false;
    if (c >= 'A' && c <= 'Z')
        upper = true;
    if (upper)
        c += 32;
    return c;
}

extern "C" int stricmp(const char* a, const char* b) {
    int diff;
    for (;;) {
        diff = LowerChar(*a) - LowerChar(*b);
        if (diff != 0 || *a == 0)
            break;
        a++;
        b++;
    }
    return diff;
}
