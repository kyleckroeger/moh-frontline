// Hierarchy-object helpers: the matrix post-multiply and the path-following
// query (both weak), and the per-frame update that runs unless the object is
// paused. CMatrix, CStaticObject and CHierObject are named by the mangled
// symbols; the members and the flag-byte bit-field view are inferred, and the
// classes are non-virtual views. The rest of the file is not part of this
// unit.
class CMatrix {
public:
    void Multiply(const CMatrix&, const CMatrix&);
    void PostMultiply(const CMatrix&);
};

class CStaticObject {
public:
    bool IsFollowingPath() const;

    unsigned char unknown000[260];
    int m_pathState;
};

class CHierObject {
public:
    void BeginUpdate(float);
    void DoBeginUpdate(float, bool);

    unsigned char unknown000[1416];
    bool paused : 1;
    unsigned char flags : 7;
};

__declspec(weak) void CMatrix::PostMultiply(const CMatrix& m) {
    Multiply(*this, m);
}

__declspec(weak) bool CStaticObject::IsFollowingPath() const {
    bool following = false;
    if (m_pathState != 0 && m_pathState != 1)
        following = true;
    return following;
}

void CHierObject::BeginUpdate(float time) {
    if (!paused)
        DoBeginUpdate(time, true);
}
