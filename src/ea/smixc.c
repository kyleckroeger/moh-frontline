// mixc: add a scaled channel into a mix buffer.
extern "C" void mixc(int count, float* source, float* dest, float gain) {
    int i;

    for (i = 0; i < count; i++)
        dest[i] = dest[i] + gain * source[i];
}
