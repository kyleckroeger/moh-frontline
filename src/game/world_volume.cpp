// CWorldVolume: the weak TestCollision defaults (no collision) for triangles,
// planes, points, world volumes and CDB objects. The names come from the
// mangled symbols; the class is a non-virtual view and the result type (bool)
// is inferred. They are inline in the original (weak symbols), so they are
// defined __declspec(weak). GetExtents follows them (the extents at +40 and
// +56, offsets inferred). The rest of the file is not part of this unit.
class CCollision;
class CTriangle;
class CPlane;
class CCDBObject;

// CVector3 view: four floats, 8-byte aligned, overlaid with two doubles. The
// union is inferred from the code, not the original declaration: GetExtents
// copies each vector as two lfd/stfd pairs, which an implicit copy through the
// double pair reproduces.
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

class CWorldVolume {
public:
    bool TestCollision(const CTriangle&, CCollision&, bool) const;
    bool TestCollision(const CPlane&, CCollision&, bool) const;
    bool TestCollision(CVector3, CCollision&, bool) const;
    bool TestCollision(const CWorldVolume&, CCollision&, bool) const;
    bool TestCollision(const CCDBObject&, CCollision&, bool) const;
    void GetExtents(CVector3&, CVector3&) const;

    unsigned char unknown00[40];
    CVector3 m_min;
    CVector3 m_max;
};

__declspec(weak) bool CWorldVolume::TestCollision(const CTriangle&, CCollision&, bool) const {
    return false;
}

__declspec(weak) bool CWorldVolume::TestCollision(const CPlane&, CCollision&, bool) const {
    return false;
}

__declspec(weak) bool CWorldVolume::TestCollision(CVector3, CCollision&, bool) const {
    return false;
}

__declspec(weak) bool CWorldVolume::TestCollision(const CWorldVolume&, CCollision&, bool) const {
    return false;
}

__declspec(weak) bool CWorldVolume::TestCollision(const CCDBObject&, CCollision&, bool) const {
    return false;
}

void CWorldVolume::GetExtents(CVector3& min, CVector3& max) const {
    min = m_min;
    max = m_max;
}
