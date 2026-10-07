// A fragment of matrix.cpp (0x8008fa84): CMatrix::TransformVector and
// TransformPoint, which pass the result, the matrix and the vector to the
// paired-single helpers PSTransformVector and PSTransformPoint. CMatrix,
// CVector3 and the helpers are named by the mangled symbols; the CVector3 view
// is as in matrix.cpp. The rest of the file is not part of this unit.
class CVector3 {
public:
    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

void PSTransformVector(float*, const float*, const float*);
void PSTransformPoint(float*, const float*, const float*);

class CMatrix {
public:
    void TransformVector(CVector3&, CVector3) const;
    void TransformPoint(CVector3&, CVector3) const;

    float m[16];
};

void CMatrix::TransformVector(CVector3& result, CVector3 v) const {
    PSTransformVector(&result.x, m, &v.x);
}

void CMatrix::TransformPoint(CVector3& result, CVector3 v) const {
    PSTransformPoint(&result.x, m, &v.x);
}
