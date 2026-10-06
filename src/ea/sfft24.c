// Converts float samples to integers clamped to +/-32767.
// Only the process entry and the word at +4 are established.
struct FT24_32STATE {
    int (*process)(void*, int, void*, void*, int);
    int field04;
};

int SFILTER_ft24_32(void*, int count, void* source, void* destination, int) {
    float* in = static_cast<float*>(source);
    int* out = static_cast<int*>(destination);
    for (int i = 0; i < count; i++) {
        int value = (int)*in;
        if (value < -32767)
            value = -32767;
        else if (value > 32767)
            value = 32767;
        *out = value;
        in++;
        out++;
    }
    return count;
}

void SFILTER_ft24_32init(FT24_32STATE* state) {
    state->process = SFILTER_ft24_32;
    state->field04 = 0;
}
