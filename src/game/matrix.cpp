// CMatrix translations, row setters and builders: Translate adds a vector to
// the position, PreTranslate adds it rotated by the matrix rows; position, up,
// front and right copy a vector's three coordinates into the matrix rows.
// BuildTrans/BuildRotZ/BuildRotX reset the matrix to identity and set the
// translation or a Z/X rotation (angle scaled to MathSinCos's fixed-point
// units). CMatrix and CVector3 are named by the mangled symbols; the row
// layout (right, front, up, position, 16 bytes each) is inferred from the
// offsets. Ident is the paired-single routine of matrix_endian, inlined here
// (inferred from the identical store sequence). The rest of the file is not
// part of this unit.
class CVector3 {
public:
    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

void MathSinCos(long, float*, float*);

class CMatrix {
public:
    void Ident();
    void BuildTrans(CVector3);
    void BuildRotZ(float);
    void BuildRotX(float);
    void Translate(CVector3);
    void PreTranslate(CVector3);
    void SetPos(CVector3);
    void SetUp(CVector3);
    void SetFront(CVector3);
    void SetRight(CVector3);

    CVector3 right;
    CVector3 front;
    CVector3 up;
    CVector3 pos;
};

void CMatrix::Translate(CVector3 v) {
    pos.x += v.x;
    pos.y += v.y;
    pos.z += v.z;
}

void CMatrix::PreTranslate(CVector3 v) {
    pos.x += right.x * v.x + front.x * v.y + up.x * v.z;
    pos.y += right.y * v.x + front.y * v.y + up.y * v.z;
    pos.z += right.z * v.x + front.z * v.y + up.z * v.z;
}

void CMatrix::SetPos(CVector3 v) {
    pos.x = v.x;
    pos.y = v.y;
    pos.z = v.z;
}

void CMatrix::SetUp(CVector3 v) {
    up.x = v.x;
    up.y = v.y;
    up.z = v.z;
}

void CMatrix::SetFront(CVector3 v) {
    front.x = v.x;
    front.y = v.y;
    front.z = v.z;
}

void CMatrix::SetRight(CVector3 v) {
    right.x = v.x;
    right.y = v.y;
    right.z = v.z;
}

inline void CMatrix::Ident() {
    register CMatrix* mtx = this;
    register float c_zero = 0.0f;
    register float c_one = 1.0f;
    register float c_01;
    register float c_10;

    asm {
        psq_st c_zero, 8(mtx), 0, 0
        ps_merge01 c_01, c_zero, c_one
        psq_st c_zero, 24(mtx), 0, 0
        ps_merge10 c_10, c_one, c_zero
        psq_st c_zero, 32(mtx), 0, 0
        psq_st c_zero, 48(mtx), 0, 0
        psq_st c_01, 16(mtx), 0, 0
        psq_st c_10, 0(mtx), 0, 0
        psq_st c_10, 40(mtx), 0, 0
        psq_st c_01, 56(mtx), 0, 0
        nop
    }
}

void CMatrix::BuildTrans(CVector3 v) {
    Ident();
    CVector3 p = v;
    float x = p.x;
    float y = p.y;
    float z = p.z;
    pos.x = x;
    pos.y = y;
    pos.z = z;
}

void CMatrix::BuildRotZ(float angle) {
    float s;
    float c;
    MathSinCos(2670176.8f * angle, &s, &c);
    Ident();
    right.x = c;
    right.y = s;
    front.x = -s;
    front.y = c;
}

void CMatrix::BuildRotX(float angle) {
    float s;
    float c;
    MathSinCos(2670176.8f * angle, &s, &c);
    Ident();
    front.y = c;
    front.z = s;
    up.y = -s;
    up.z = c;
}
