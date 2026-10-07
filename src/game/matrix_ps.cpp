// A fragment of matrix.cpp (0x8008fa08): CMatrix::TransformToLocal (the point
// relative to the matrix position through the paired-single helper
// PSTransformToLocal, with w set to 1; 1.0f is an entry of the file's .sdata2
// pool), TransformVector and
// TransformPoint, which pass the result, the matrix and the vector to the
// paired-single helpers PSTransformVector and PSTransformPoint, and Transform
// (a point to homogeneous coordinates, in C). CMatrix,
// CVector3 and the helpers are named by the mangled symbols; the CVector3 view
// is as in matrix.cpp. The rest of the file is not part of this unit.
class CVector3 {
public:
    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

// Inferred: a four-float vector.
class CVector4 {
public:
    float x;
    float y;
    float z;
    float w;
};

void PSTransformToLocal(float*, const float*, const float*);
void PSTransformVector(float*, const float*, const float*);
void PSTransformPoint(float*, const float*, const float*);

class CMatrix {
public:
    CVector3 TransformToLocal(CVector3) const;
    void TransformVector(CVector3&, CVector3) const;
    void TransformPoint(CVector3&, CVector3) const;
    void Transform(CVector4&, CVector3) const;

    float m[16];
};

CVector3 CMatrix::TransformToLocal(CVector3 v) const {
    CVector3 result;
    CVector3 delta;
    delta.z = v.z - m[14];
    delta.y = v.y - m[13];
    delta.x = v.x - m[12];
    PSTransformToLocal(&result.x, m, &delta.x);
    result.w = 1.0f;
    return result;
}

void CMatrix::TransformVector(CVector3& result, CVector3 v) const {
    PSTransformVector(&result.x, m, &v.x);
}

void CMatrix::TransformPoint(CVector3& result, CVector3 v) const {
    PSTransformPoint(&result.x, m, &v.x);
}

void CMatrix::Transform(CVector4& result, CVector3 v) const {
    result.x = v.x * m[0] + v.y * m[4] + v.z * m[8] + m[12];
    result.y = v.x * m[1] + v.y * m[5] + v.z * m[9] + m[13];
    result.z = v.x * m[2] + v.y * m[6] + v.z * m[10] + m[14];
    result.w = v.x * m[3] + v.y * m[7] + v.z * m[11] + m[15];
}
