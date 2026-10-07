// A fragment of matrix.cpp (0x8008fc50): CMatrix::Multiply passes the result
// and the two matrices to the paired-single helper PSMultMatrix. CMatrix and
// the helper are named by the mangled symbols. The rest of the file is not
// part of this unit.
void PSMultMatrix(float*, const float*, const float*);

class CMatrix {
public:
    void Multiply(const CMatrix&, const CMatrix&);

    float m[16];
};

void CMatrix::Multiply(const CMatrix& a, const CMatrix& b) {
    PSMultMatrix(m, a.m, b.m);
}
