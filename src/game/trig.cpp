// Angle helpers on 24-bit fixed-point angles (one turn is 0x1000000).
// Fragment: the original file continues with MathLLAngleInit and its Taylor
// tables (draft in scratch/game/trig.cpp).
extern "C" {
double atan2(double, double);
double sin(double);
double cos(double);
}

long MathArcTan2(float y, float x) {
    return 2670176.8f * (float)atan2(y, x);
}

void MathSinCos(long angle, float* s, float* c) {
    float radians = 6.2831855f * angle / 16777216.0f;

    *s = sin(radians);
    *c = cos(radians);
}

float MathCosf(float x) {
    return cos(x);
}

float MathSinf(float x) {
    return sin(x);
}
