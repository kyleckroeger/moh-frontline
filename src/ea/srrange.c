// randrange: a random value in [-range, range), with range clamped to
// [0, 0x10000].
int iSNDrandom(void);

int randrange(int range) {
    if (range > 0x10000)
        range = 0x10000;
    else if (range < 0)
        range = 0;
    int value = (iSNDrandom() & 0x7fff) - 0x4000;

    return value * range >> 14;
}
