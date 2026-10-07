// A fragment of matrix.cpp (0x8008fc0c): CMatrix::Perspective (a frustum of the
// given half extents scaled by the near distance, with the far plane at 1000;
// the fourth argument is unused and 1000.0f is an entry of the file's .sdata2
// pool), then CMatrix::Multiply, which passes the result
// and the two matrices to the paired-single helper PSMultMatrix. CMatrix and
// the helper are named by the mangled symbols. The rest of the file is not
// part of this unit.
void PSMultMatrix(float*, const float*, const float*);
extern "C" void C_MTXFrustum(float (*)[4], float, float, float, float, float, float);

class CMatrix {
public:
    void Perspective(float, float, float, float);
    void Multiply(const CMatrix&, const CMatrix&);

    float m[16];
};

void CMatrix::Perspective(float x, float y, float n, float) {
    C_MTXFrustum((float (*)[4])m, y * n, -y * n, -x * n, x * n, n, 1000.0f);
}

void CMatrix::Multiply(const CMatrix& a, const CMatrix& b) {
    PSMultMatrix(m, a.m, b.m);
}
