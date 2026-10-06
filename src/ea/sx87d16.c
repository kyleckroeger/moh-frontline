// decode16x87: convert 16-bit PCM samples to floats.
extern "C" void decode16x87(int count, short* source, float* dest) {
    int i;

    for (i = 0; i < count; i++)
        dest[i] = source[i];
}
