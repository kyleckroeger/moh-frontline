// sqrt(1 + x) by its binomial series; each term is the previous one times a
// coefficient and x.
float SNDI_rootof1plusx(float x) {
    float t1 = 0.5f * x;
    float t2 = -0.25f * t1 * x;
    float t3 = -0.5f * t2 * x;
    float t4 = -0.625f * t3 * x;
    float t5 = -0.7f * t4 * x;
    float t6 = -0.75f * t5 * x;
    float t7 = -0.7857f * t6 * x;
    return 1.0f + t1 + t2 + t3 + t4 + t5 + t6 + t7;
}
