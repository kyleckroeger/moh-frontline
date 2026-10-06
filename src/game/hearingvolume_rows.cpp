// A fragment of hearingvolume.cpp (0x80098b04): CHearingVolume's row accessors (GetUpward, GetForward, GetRightward, GetPosition),
// each copying one row of the object's transform into the result. The file
// name is this project's; the original record is hearingvolume.cpp and the functions
// around these are not reconstructed. CHearingVolume is named by the mangled symbols;
// it is an inferred non-virtual view with only the transform declared, at the
// offset the copies read (+48), and the by-value row getters are inferred
// inline helpers.
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

class CHearingVolume {
public:
    void GetUpward(CVector3&) const;
    void GetForward(CVector3&) const;
    void GetRightward(CVector3&) const;
    void GetPosition(CVector3&) const;

    CVector3 GetRightRow() const { return m_tm.right; }
    CVector3 GetForwardRow() const { return m_tm.forward; }
    CVector3 GetUpRow() const { return m_tm.up; }
    CVector3 GetPositionRow() const { return m_tm.position; }

    unsigned char unknown00[48];
    CMatrix m_tm;
};

void CHearingVolume::GetUpward(CVector3& v) const {
    v = GetUpRow();
}

void CHearingVolume::GetForward(CVector3& v) const {
    v = GetForwardRow();
}

void CHearingVolume::GetRightward(CVector3& v) const {
    v = GetRightRow();
}

void CHearingVolume::GetPosition(CVector3& v) const {
    v = GetPositionRow();
}
