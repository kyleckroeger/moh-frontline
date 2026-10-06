// A fragment of sphere.cpp (0x800cb4f0): CVolSphere's double-dispatch
// collision tests. Against CDB objects, capsules, world volumes and CSG
// volumes the collision order is swapped and the other volume tests against
// the sphere; against a fiber the collision's line flag is set and the sphere
// tests the fiber's line; an animated volume is tested as a sphere. The file
// name is this project's; the original record is sphere.cpp (sphere.cpp holds
// the box and generic tests onward) and the sphere-sphere test between them
// is not reconstructed. The classes come from the mangled symbols; IVolume's
// virtual functions are declared in the order of __vt__7IVolume, CVolSphere
// redeclares them (its destructor first, defined elsewhere, so its virtual
// table is not emitted here), CAnimatedVolume is taken to derive from
// CVolSphere (the call it makes implies it), and the collision flag is an
// inferred bit-field view.
class CMatrix;
class CVector3;
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

class CCollision {
public:
    void SwapOrder();

    unsigned char unknown00[16];
    unsigned char m_line : 1;
    unsigned char unknown10 : 7;
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

class CCDBObject : public IVolume {};
class CVolCapsule : public IVolume {};
class CWorldVolume : public IVolume {};
class CCSGVolume : public IVolume {};

class CVolFiber : public IVolume {
public:
    const CLine3& GetLine() const;
};

class CVolSphere : public IVolume {
public:
    virtual ~CVolSphere();
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
    bool TestCollision(const CCSGVolume&, CCollision&, bool) const;
};

class CAnimatedVolume : public CVolSphere {};

bool CVolSphere::TestCollision(const CCDBObject& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    return other.TestCollision(*this, collision, flag);
}

bool CVolSphere::TestCollision(const CVolFiber& other, CCollision& collision, bool flag) const {
    collision.m_line = 1;
    return TestCollision(other.GetLine(), collision, flag);
}

bool CVolSphere::TestCollision(const CVolCapsule& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    return other.TestCollision(*this, collision, flag);
}

bool CVolSphere::TestCollision(const CWorldVolume& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    return other.TestCollision(*this, collision, flag);
}

bool CVolSphere::TestCollision(const CCSGVolume& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    return other.TestCollision(*this, collision, flag);
}

bool CVolSphere::TestCollision(const CAnimatedVolume& other, CCollision& collision, bool flag) const {
    return TestCollision((const CVolSphere&)other, collision, flag);
}
