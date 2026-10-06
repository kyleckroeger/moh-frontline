// A fragment of camera.cpp (0x80079ee8): CCamera's transform wrappers, from
// Orthonormalize to CommitUpdate, each forwarding to the object's transform
// (a CMatrix at +64) and then clearing two flags (+0x141 and +0x143; their
// meaning is unknown), with IsVisible and IsDrawEnabled (both false) and an
// empty Draw among them. The file
// name is this project's; the original record is camera.cpp and the functions
// around these are not reconstructed. CCamera and CMatrix's methods are named by
// the mangled symbols; CCamera is an inferred non-virtual view with only the
// transform declared, the bodies are inferred from the calls, and the by-value
// row getters are inferred inline helpers.
//
// CVector3 view: four floats, 8-byte aligned, overlaid with two doubles. The
// union is inferred from the code, not the original declaration: vectors are
// copied as two lfd/stfd pairs, which an implicit copy through the double pair
// reproduces.
class CVector3 {
public:
    union {
        struct {
            float x;
            float y;
            float z;
            float w;
        };
        double pair[2];
    };
} __attribute__((aligned(8)));

class CMatrix {
public:
    CMatrix& operator=(const CMatrix&);
    void Orthonormalize();
    void RotateX(float);
    void RotateY(float);
    void RotateZ(float);
    void Rotate(CVector3, float);
    void Translate(CVector3);
    void SetRight(CVector3);
    void SetFront(CVector3);
    void SetUp(CVector3);
    void SetPos(CVector3);
    void Multiply(const CMatrix&, const CMatrix&);
    void Ident();

    CVector3 right;
    CVector3 forward;
    CVector3 up;
    CVector3 position;
};

class CDrawContext;

class CCamera {
public:
    bool IsVisible(CDrawContext&) const;
    bool IsDrawEnabled() const;
    void Draw(CDrawContext&);
    void Orthonormalize();
    void Yaw(float);
    void Roll(float);
    void Pitch(float);
    void Rotate(CVector3, float);
    void Move(CVector3);
    void SetBasis(CVector3, CVector3, CVector3);
    void SetPosition(CVector3);
    void SetTMLocalToWorld(const CMatrix&);
    void Transform(const CMatrix&);
    void Reset();
    void PreTransform(const CMatrix&);
    void GetUpward(CVector3&) const;
    void GetForward(CVector3&) const;
    void GetRightward(CVector3&) const;
    void GetPosition(CVector3&) const;
    void GetTMLocalToWorld(CMatrix&) const;
    void CommitUpdate();

    CVector3 UpRow() const { return m_tm.up; }
    CVector3 ForwardRow() const { return m_tm.forward; }
    CVector3 RightRow() const { return m_tm.right; }
    CVector3 PositionRow() const { return m_tm.position; }

    unsigned char unknown00[64];
    CMatrix m_tm;
    unsigned char unknown80[193];
    bool m_flag141;
    unsigned char unknown142[1];
    bool m_flag143;
};

void CCamera::Orthonormalize() {
    m_tm.Orthonormalize();
    m_flag141 = false;
    m_flag143 = false;
}

void CCamera::Yaw(float angle) {
    m_tm.RotateZ(angle);
    m_flag141 = false;
    m_flag143 = false;
}

void CCamera::Roll(float angle) {
    m_tm.RotateY(angle);
    m_flag141 = false;
    m_flag143 = false;
}

void CCamera::Pitch(float angle) {
    m_tm.RotateX(angle);
    m_flag141 = false;
    m_flag143 = false;
}

void CCamera::Rotate(CVector3 axis, float angle) {
    m_tm.Rotate(axis, angle);
    m_flag141 = false;
    m_flag143 = false;
}

void CCamera::Move(CVector3 delta) {
    m_tm.Translate(delta);
    m_flag141 = false;
    m_flag143 = false;
}

void CCamera::SetBasis(CVector3 right, CVector3 forward, CVector3 up) {
    m_tm.SetRight(right);
    m_tm.SetFront(forward);
    m_tm.SetUp(up);
    m_flag141 = false;
    m_flag143 = false;
}

void CCamera::SetPosition(CVector3 position) {
    m_tm.SetPos(position);
    m_flag141 = false;
    m_flag143 = false;
}

void CCamera::SetTMLocalToWorld(const CMatrix& matrix) {
    m_tm = matrix;
    m_flag141 = false;
    m_flag143 = false;
}

void CCamera::Transform(const CMatrix& matrix) {
    m_tm.Multiply(m_tm, matrix);
    m_flag141 = false;
    m_flag143 = false;
}

void CCamera::Reset() {
    m_tm.Ident();
    m_flag141 = false;
    m_flag143 = false;
}

void CCamera::PreTransform(const CMatrix& matrix) {
    m_tm.Multiply(matrix, m_tm);
    m_flag141 = false;
    m_flag143 = false;
}

bool CCamera::IsVisible(CDrawContext&) const {
    return false;
}

bool CCamera::IsDrawEnabled() const {
    return false;
}

void CCamera::GetUpward(CVector3& v) const {
    v = UpRow();
}

void CCamera::GetForward(CVector3& v) const {
    v = ForwardRow();
}

void CCamera::GetRightward(CVector3& v) const {
    v = RightRow();
}

void CCamera::GetPosition(CVector3& v) const {
    v = PositionRow();
}

void CCamera::GetTMLocalToWorld(CMatrix& matrix) const {
    matrix = m_tm;
}

void CCamera::Draw(CDrawContext&) {
}

void CCamera::CommitUpdate() {
}
