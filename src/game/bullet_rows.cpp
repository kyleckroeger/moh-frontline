// A fragment of bullet.cpp (0x800ccb84): CBullet's row accessors (GetRightward, GetUp),
// each copying one row of the object's transform into the result. The file
// name is this project's; the original record is bullet.cpp and the functions
// around these are not reconstructed. CBullet is named by the mangled symbols;
// it is an inferred non-virtual view with only the transform declared, at the
// offset the copies read (+64), and the by-value row getters are inferred
// inline helpers. Both accessors are weak in the image (inline in the
// original class), so they are defined __declspec(weak).
//
// CVector3 view: four floats, 8-byte aligned, overlaid with two doubles. The
// union is inferred from the code, not the original declaration: the row
// copies move each vector as two lfd/stfd pairs, which an implicit copy
// through the double pair reproduces.
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
    CVector3 right;
    CVector3 forward;
    CVector3 up;
    CVector3 position;
};

class CBullet {
public:
    void GetRightward(CVector3&) const;
    void GetUp(CVector3&) const;

    CVector3 GetForwardRow() const { return m_tm.forward; }
    CVector3 GetPositionRow() const { return m_tm.position; }

    unsigned char unknown00[64];
    CMatrix m_tm;
};

__declspec(weak) void CBullet::GetRightward(CVector3& v) const {
    v = GetForwardRow();
}

__declspec(weak) void CBullet::GetUp(CVector3& v) const {
    v = GetPositionRow();
}
