// SNDover: whether a sound handle has finished (no voice plays it).
int SNDVOICEI_get(int);

extern "C" int SNDover(int handle) {
    return SNDVOICEI_get(handle) < 0;
}
