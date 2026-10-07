// A fragment of player.cpp (0x800a4d48): the weak std::sqrtf, MSL's inline
// square root (three Newton-Raphson steps from frsqrte), emitted as a weak
// copy in this file, so it is defined __declspec(weak). Its constants are
// entries of the file's .sdata2 pool.
namespace std {

__declspec(weak) float sqrtf(float x)
{
    const double _half = .5;
    const double _three = 3.0;
    volatile float y;
    if (x > 0.0f)
    {
        double guess = __frsqrte((double)x);
        guess = _half*guess*(_three - guess*guess*x);
        guess = _half*guess*(_three - guess*guess*x);
        guess = _half*guess*(_three - guess*guess*x);
        y = (float)(x*guess);
        return y;
    }
    return x;
}

}
