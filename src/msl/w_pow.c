/* fdlibm w_pow.c as built for MSL (_IEEE_LIBM): pow calls __ieee754_pow
 * directly, with no error handling. */
double __ieee754_pow(double, double);

double pow(double x, double y) {
    return __ieee754_pow(x, y);
}
