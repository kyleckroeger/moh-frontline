// CVolSphere collision tests against boxes and generic volumes (the other
// volume tests against the sphere, with the collision order swapped) and the
// radius accessor. The names come from the mangled symbols; IVolume's virtual
// functions are declared in the order of __vt__7IVolume, and CVolSphere is a
// non-virtual view whose radius offset is inferred. The centre accessors that
// follow copy a 16-byte vector as doublewords and are not part of this unit,
// nor are the tests before these.
class CMatrix;
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

class CVector3 {
public:
    float x;
    float y;
    float z;
    float w;
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

class CVolBox : public IVolume {
};

class CVolSphere {
public:
    bool TestCollision(const CVolBox&, CCollision&, bool) const;
    bool TestCollision(const IVolume&, CCollision&, bool) const;
    float GetRadius() const;

    unsigned char unknown00[12];
    float m_radius;
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
