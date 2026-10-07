// CMatrix translations and row setters: Translate adds a vector to the
// position, PreTranslate adds it rotated by the matrix rows; position, up,
// front and right copy a vector's three coordinates into the matrix rows. CMatrix and CVector3 are named by the
// mangled symbols; the row layout (right, front, up, position, 16 bytes each)
// is inferred from the offsets. The rest of the file is not part of this unit.
class CVector3 {
public:
    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CMatrix {
public:
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
