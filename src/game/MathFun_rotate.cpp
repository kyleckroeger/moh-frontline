// The end of MathFun.cpp (0x8009340c): MathFunNormalizeAngleNegativePiToPi
// (removes whole turns, then brings the angle into -pi..pi),
// MathFunRotateAboutZ and MathFunRotateAboutY (rotate a coordinate pair in
// place by an angle, through the library's double sin and cos) and
// MathFunAtan2F (the quadrant-corrected angle of a y/x pair from atan; note
// the right half-plane uses -y). The constants are entries of the file's
// .sdata2 pool. The functions are named by their symbols; the parameter
// meanings are inferred. The angle-difference helpers before these are not
// part of the unit.
extern "C" {
double sin(double);
double cos(double);
double atan(double);
}

float MathFunNormalizeAngleNegativePiToPi(float angle) {
    angle -= 3.1415927f * (float)((int)(angle / 6.2831855f) * 2);
    if (angle > 3.1415927f)
        angle -= 6.2831855f;
    else if (angle < -3.1415927f)
        angle += 6.2831855f;
    return angle;
}

void MathFunRotateAboutZ(float* x, float* y, float angle) {
    float s = sin(angle);
    float c = cos(angle);
    float ox = *x;
    float oy = *y;
    *x = ox * c - oy * s;
    *y = ox * s + oy * c;
}

void MathFunRotateAboutY(float* z, float* x, float angle) {
    float s = sin(angle);
    float c = cos(angle);
    float ox = *x;
    float oz = *z;
    *x = ox * c - oz * s;
    *z = ox * s + oz * c;
}

float MathFunAtan2F(float y, float x) {
    if (x > 0.0f)
        return atan(-y / x);
    if (x < 0.0f) {
        if (y < 0.0f)
            return 3.1415927f - (float)atan(y / x);
        return -3.1415927f - (float)atan(y / x);
    }
    if (y <= 0.0f)
        return 1.5707964f;
    return -1.5707964f;
}
