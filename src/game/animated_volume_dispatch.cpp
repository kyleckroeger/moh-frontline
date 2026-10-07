// A fragment of animated_volume.cpp (0x800bfa2c): CAnimatedVolume::Create
// (a new volume from DWI_alloc: the inline constructors set the IVolume,
// CVolSphere and CAnimatedVolume virtual tables, construct the array of 24
// boxes and clear the members), TransformedCopy (the sphere's copy, then the
// members, with the flag cleared) and the double-dispatch collision tests
// (against CAnimatedVolume, CCSGVolume, CWorldVolume, CVolFiber). Swapped
// tests reverse the collision order and let the other volume test this one;
// fiber tests set the collision's line flag and test the fiber's line. The
// file name is this project's; the original record is animated_volume.cpp and
// the functions around these are not part of this unit. The classes come from
// the mangled symbols; IVolume's virtual functions are declared in the order
// of __vt__7IVolume and the volume classes redeclare them with their
// destructors first (defined elsewhere, so their virtual tables are not
// emitted here); CAnimatedVolume derives from CVolSphere (its tests call the
// sphere's). The members, sizes, the collision flag bit-field and the inline
// class operator new (DWI_alloc with no name and flag 1024) are inferred.
class CMatrix;
class CVector3;
class CTriangle;
class CPlane;
class CLine3;
class CVolBox;
void* DWI_alloc(const char*, int, int);
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

class CCSGVolume : public IVolume {};

class CVolSphere : public IVolume {
public:
    virtual ~CVolSphere();
    static void* operator new(unsigned long size) { return DWI_alloc(0, size, 1024); }
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

    unsigned char unknown04[28];
};

class CVolBox : public IVolume {
public:
    virtual ~CVolBox();
    CVolBox();

    float m_values[3];
    double m_vectors[8];
};

class CVolFiber : public IVolume {
public:
    const CLine3& GetLine() const;
};

class CWorldVolume : public IVolume {
public:
};

class CAnimatedVolume : public CVolSphere {
public:
    virtual ~CAnimatedVolume();
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

    CAnimatedVolume() {
        m_count36 = 0;
        m_flag = 0;
        m_value40 = 0;
        m_value1968 = 0;
    }

    unsigned char m_flag;
    int m_count36;
    int m_value40;
    CVolBox m_boxes[24];
    int m_value1968;
    unsigned char unknown7b4[12];
};

void CAnimatedVolume::TransformedCopy(const IVolume& volume, const CMatrix& matrix) {
    const CAnimatedVolume& other = (const CAnimatedVolume&)volume;

    CVolSphere::TransformedCopy(volume, matrix);
    m_count36 = other.m_count36;
    m_value40 = other.m_value40;
    m_flag = 0;
    m_value1968 = other.m_value1968;
}

IVolume* CAnimatedVolume::Create() const {
    return new CAnimatedVolume;
}

bool CAnimatedVolume::TestCollision(const CAnimatedVolume& other, CCollision& collision, bool flag) const {
    return CVolSphere::TestCollision((const CVolSphere&)other, collision, flag);
}

bool CAnimatedVolume::TestCollision(const CCSGVolume& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    return other.TestCollision(*this, collision, flag);
}

bool CAnimatedVolume::TestCollision(const CWorldVolume& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    return other.TestCollision(*this, collision, flag);
}

bool CAnimatedVolume::TestCollision(const CVolFiber& other, CCollision& collision, bool flag) const {
    collision.m_line = 1;
    return TestCollision(other.GetLine(), collision, flag);
}
