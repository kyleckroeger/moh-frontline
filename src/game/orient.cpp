// Angle interpolation on 24-bit fixed-point orientations (one full turn is
// 0x1000000). The shorter way round is taken by unwrapping one endpoint.
extern "C" {

void OrientLerpAngle(int* result, int target, int source, float fraction) {
    int from = source & 0xffffff;
    int to = target & 0xffffff;

    if (to - from > 0x800000)
        from += 0x1000000;
    else if (to - from < -0x800000)
        to += 0x1000000;
    int delta = to - from;
    *result = (((long long)delta * (int)(fraction * 16777216.0f)) >> 24) + from;
}

void Orient3Lerp(int* result, int* target, int* source, float fraction) {
    long long scale = (int)(fraction * 16777216.0f);
    int i;

    for (i = 0; i < 3; i++) {
        int from = source[i] & 0xffffff;
        int to = target[i] & 0xffffff;
        int delta = to - from;

        if (delta > 0x800000)
            from += 0x1000000;
        else if (delta < -0x800000)
            to += 0x1000000;
        result[i] = (((long long)(to - from) * scale) >> 24) + from;
    }
}
}
