// CWorldVolume: the weak TestCollision defaults (no collision) for triangles,
// planes, points, world volumes and CDB objects. The names come from the
// mangled symbols; the class is a non-virtual view and the result type (bool)
// is inferred. They are inline in the original (weak symbols), so they are
// defined __declspec(weak). GetExtents follows them (the extents at +40 and
// +56, offsets inferred), then the TestCollision overloads: CSG and generic
// volumes swap the collision order and let the other volume test (IVolume's
// virtual functions declared in the order of __vt__7IVolume), the others go
// through the TestCollisionWithSubVolumes template (instantiated elsewhere).
// The rest of the file is not part of this unit.
class CCollision;
class CTriangle;
class CPlane;
class CCDBObject;
class CMatrix;
class CLine3;
class CVolBox;
class CVolSphere;
class CVolCapsule;
class CVolFiber;
class CAnimatedVolume;
class CWorldVolume;

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

class CCollision {
public:
    void SwapOrder();
};

class IVolume {
public:
    virtual ~IVolume();
    virtual IVolume* Create() const;
    virtual void TransformedCopy(const IVolume&, const CMatrix&);
    virtual bool TestCollision(const IVolume&, CCollision&, bool) const;
    virtual bool TestCollision(const CVolBox&, CCollision&, bool) const;
    virtual bool TestCollision(const CVolSphere&, CCollision&, bool) const;
    virtual bool TestCollision(const CVolCapsule&, CCollision&, bool) const;
    virtual bool TestCollision(const CCDBObject&, CCollision&, bool) const;
    virtual bool TestCollision(const CVolFiber&, CCollision&, bool) const;
    virtual bool TestCollision(const CAnimatedVolume&, CCollision&, bool) const;
    virtual bool TestCollision(const CWorldVolume&, CCollision&, bool) const;
    virtual bool TestCollision(CVector3, CCollision&, bool) const;
    virtual bool TestCollision(const CLine3&, CCollision&, bool) const;
    virtual bool TestCollision(const CPlane&, CCollision&, bool) const;
    virtual bool TestCollision(const CTriangle&, CCollision&, bool) const;
};

class CCSGVolume : public IVolume {};

class CWorldVolume {
public:
    bool TestCollision(const CTriangle&, CCollision&, bool) const;
    bool TestCollision(const CPlane&, CCollision&, bool) const;
    bool TestCollision(CVector3, CCollision&, bool) const;
    bool TestCollision(const CWorldVolume&, CCollision&, bool) const;
    bool TestCollision(const CCDBObject&, CCollision&, bool) const;
    void GetExtents(CVector3&, CVector3&) const;
    bool TestCollision(const CCSGVolume&, CCollision&, bool) const;
    bool TestCollision(const CAnimatedVolume&, CCollision&, bool) const;
    bool TestCollision(const CVolFiber&, CCollision&, bool) const;
    bool TestCollision(const CLine3&, CCollision&, bool) const;
    bool TestCollision(const CVolCapsule&, CCollision&, bool) const;
    bool TestCollision(const CVolSphere&, CCollision&, bool) const;
    bool TestCollision(const CVolBox&, CCollision&, bool) const;
    bool TestCollision(const IVolume&, CCollision&, bool) const;
    template <class T> bool TestCollisionWithSubVolumes(const T&, CCollision&, bool) const;

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

bool CWorldVolume::TestCollision(const CCSGVolume& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    return other.TestCollision(*this, collision, flag);
}

bool CWorldVolume::TestCollision(const CAnimatedVolume& other, CCollision& collision, bool flag) const {
    return TestCollisionWithSubVolumes(other, collision, flag);
}

bool CWorldVolume::TestCollision(const CVolFiber& other, CCollision& collision, bool flag) const {
    return TestCollisionWithSubVolumes(other, collision, flag);
}

bool CWorldVolume::TestCollision(const CLine3& other, CCollision& collision, bool flag) const {
    return TestCollisionWithSubVolumes(other, collision, flag);
}

bool CWorldVolume::TestCollision(const CVolCapsule& other, CCollision& collision, bool flag) const {
    return TestCollisionWithSubVolumes(other, collision, flag);
}

bool CWorldVolume::TestCollision(const CVolSphere& other, CCollision& collision, bool flag) const {
    return TestCollisionWithSubVolumes(other, collision, flag);
}

bool CWorldVolume::TestCollision(const CVolBox& other, CCollision& collision, bool flag) const {
    return TestCollisionWithSubVolumes(other, collision, flag);
}

bool CWorldVolume::TestCollision(const IVolume& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    return other.TestCollision(*this, collision, flag);
}
