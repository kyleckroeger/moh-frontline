// Sine by Taylor series after reducing the argument below 2*pi.
float SNDI_sin(float x) {
    while (x > 6.2831855f)
        x -= 6.2831855f;
    float x2 = x * x;
    float x3 = x2 * x;
    float x5 = x2 * x3;
    float x7 = x2 * x5;
    float x9 = x2 * x7;
    float x11 = x2 * x9;
    float x13 = x2 * x11;
    return x - 0.16666667f * x3 + 0.008333330f * x5 - 0.0001984127f * x7 + 2.7557319e-06f * x9 -
           2.5052108e-08f * x11 + 1.6059e-10f * x13;
}
