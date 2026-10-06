int SNDI_findprime(int, int);

// A prime-based length rounded to a multiple of 16.
int SNDI_findmult16(int a, int b) {
    return SNDI_findprime(a, b / 16) * 16;
}
