// SNDMEM_gethighwater: the highest share (percent) of sound memory in use.
// sndgs's layout is not known; the fields are read through inferred offsets.
extern "C" char sndgs[];

extern "C" int SNDMEM_gethighwater(void) {
    int* pool = *(int**)(sndgs + 516);
    int size = pool[2];

    return (size - pool[4]) * 100 / size;
}
