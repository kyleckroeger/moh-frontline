// Math helpers: percentages and random numbers on top of the library's
// random() (31 random bits), fixed-point 64-bit percent tests and a
// close-to-zero test. Parameter meanings are inferred from the code.
extern "C" {
long random(void);
void srand(unsigned int);
}

bool g_bMathFunRandomSeeded;

unsigned char MathFunTestPercent(long long percent, long long chance) {
    return percent < chance;
}

long long MathFunGetRandomPercent() {
    return ((long long)(unsigned long)(random() & 0x7FFFFFFF) * 100) >> 31;
}

float MathFunRandomRealSigned(float min, float max) {
    float value = min + (max - min) * (4.656613e-10f * (unsigned long)(random() & 0x7FFFFFFF));

    value *= (int)((random() & 2) - 1);
    return value;
}

float MathFunRandomReal(float min, float max) {
    return min + (max - min) * (4.656613e-10f * (unsigned long)(random() & 0x7FFFFFFF));
}

long long MathFunRandomI64(long long min, long long max) {
    return min + (((max - min + 1) * (long long)(unsigned long)(random() & 0x7FFFFFFF)) >> 31);
}

unsigned long MathFunRandomUI32Raw() {
    return random() & 0x7FFFFFFF;
}

void MathFunSRandom(unsigned int seed) {
    srand(seed);
    g_bMathFunRandomSeeded = true;
}

bool MathFunCloseToZero(float value, float epsilon) {
    if (value > epsilon)
        return false;
    return !(value < -epsilon);
}

// MathFunGetTiltAngleDiffNoRoll, MathFunGetPanAngleDiffNoRoll, the angle
// normalisation, the two rotations and MathFunAtan2F follow in the original
// file and are not reconstructed.
