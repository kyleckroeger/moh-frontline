// CVolSphere collision tests against boxes and generic volumes (the other
// volume tests against the sphere, with the collision order swapped) and the
// radius and centre accessors and TransformedCopy (the other sphere's centre
// through the matrix, and its radius). The names come from the mangled
// symbols; IVolume's virtual functions are declared in the order of
// __vt__7IVolume, and CVolSphere is a non-virtual view whose radius and centre
// offsets are inferred. Create, the assignment and the destructor after these
// are not part of this unit (they need the class's virtual table), nor are the
// tests before these.
class CDrawContext;
class CTriangle;
class CPlane;
class CLine3;
class CVolBox;
class CVolSphere;
class CVolCapsule;
class CVolFiber;
class CCDBObject;
class CWorldVolume;
class CAnimatedVolume;

// CVector3 view: four floats, 8-byte aligned, overlaid with two doubles. The
// union is inferred from the code, not the original declaration: the centre
// is copied as two lfd/stfd pairs, which an implicit copy through the double
// pair reproduces.
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
    void TransformPoint(CVector3&, CVector3) const;
};

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

class CVolBox : public IVolume {
};

class CVolSphere {
public:
    bool TestCollision(const CVolBox&, CCollision&, bool) const;
    bool TestCollision(const IVolume&, CCollision&, bool) const;
    float GetRadius() const;
    CVector3 GetCenter() const;
    void SetRadius(float);
    void SetCenter(CVector3);
    void TransformedCopy(const IVolume&, const CMatrix&);

    unsigned char unknown00[12];
    float m_radius;
    CVector3 m_center;
};

bool CVolSphere::TestCollision(const CVolBox& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    return other.TestCollision(*this, collision, flag);
}

bool CVolSphere::TestCollision(const IVolume& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    return other.TestCollision(*this, collision, flag);
}

float CVolSphere::GetRadius() const {
    return m_radius;
}

CVector3 CVolSphere::GetCenter() const {
    return m_center;
}

void CVolSphere::SetRadius(float radius) {
    m_radius = radius;
}

void CVolSphere::SetCenter(CVector3 center) {
    m_center = center;
}

void CVolSphere::TransformedCopy(const IVolume& other, const CMatrix& matrix) {
    const CVolSphere& sphere = (const CVolSphere&)other;
    matrix.TransformPoint(m_center, sphere.m_center);
    m_radius = sphere.m_radius;
}
