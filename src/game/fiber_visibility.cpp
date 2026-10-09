// A fragment of fiber.cpp (0x800c9440): CVolFiber::TestVisibility. A box
// volume is built, the fiber's extents are queried through its virtual
// GetExtents (the result is not used), the box is set to the corners of the
// fiber's start and end points and its visibility test is returned. The file
// name is this project's; the original record is fiber.cpp. The classes and
// functions are named by the mangled symbols; the members and the 16-byte
// vector view (the frame is realigned to 16 bytes) are inferred, and the
// result type is inferred. CVolFiber declares its destructor (defined elsewhere)
// first so its table stays elsewhere; the box's table is external.
class CMatrix;
class CCollision;
class CDrawContext;
class CTriangle;
class CPlane;
class CLine3;
class CWorldVolume;
class CAnimatedVolume;
class CCDBObject;
class CVolCapsule;
class CVolSphere;

struct VECTOR3VIEW {
    double pair[2];
} __attribute__((aligned(16)));

class CVector3 {
public:
    VECTOR3VIEW v;
};

class CVolFiber;
class CVolBox;

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
    virtual bool TestVisibility(CDrawContext&) const;
    virtual void GetExtents(CVector3&, CVector3&) const;
};

class CVolBox : public IVolume {
public:
    CVolBox() {}
    virtual ~CVolBox();
    virtual bool TestVisibility(CDrawContext&) const;
    void SetCorners(CVector3, CVector3);

    float m_halfWidth;
    float m_halfDepth;
    float m_halfHeight;
    CVector3 m_basis[3];
    CVector3 m_center;
};

class CVolFiber : public IVolume {
public:
    virtual ~CVolFiber();
    virtual bool TestVisibility(CDrawContext&) const;
    virtual void GetExtents(CVector3&, CVector3&) const;

    unsigned char unknown04[12];
    CVector3 m_start;
    CVector3 m_end;
};

bool CVolFiber::TestVisibility(CDrawContext& context) const {
    CVolBox box;
    CVector3 min;
    CVector3 max;
    GetExtents(min, max);
    box.SetCorners(m_start, m_end);
    return box.TestVisibility(context);
}
