// Cosine by Taylor series after reducing the argument below 2*pi.
float SNDI_cos(float x) {
    while (x > 6.2831855f)
        x -= 6.2831855f;
    float x2 = x * x;
    float x4 = x2 * x2;
    float x6 = x2 * x4;
    float x8 = x4 * x4;
    float x10 = x2 * x8;
    float x12 = x6 * x6;
    return 1.0f - 0.5f * x2 + 0.041666668f * x4 - 0.0013888889f * x6 + 2.4801588e-05f * x8 - 2.755732e-07f * x10 +
           2.0876758e-09f * x12;
}
