// A fragment of sphere.cpp (0x800cbabc): CVolSphere::Create allocates a new,
// default-constructed sphere (32 bytes) through DWI_alloc (no name, flag 1024;
// the inline class operator new is inferred from the call); the inline
// constructors set the IVolume and CVolSphere virtual tables. IVolume and
// CVolSphere are named by the mangled symbols; IVolume's virtual functions are
// declared in the order of __vt__7IVolume, CVolSphere declares its destructor
// first (defined elsewhere, so its virtual table is not emitted here), and
// its data is an inferred block of the size the allocation shows. The rest of
// the file is not part of this unit.
void* DWI_alloc(const char*, int, int);

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
class CCollision;

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

class CVolSphere : public IVolume {
public:
    virtual ~CVolSphere();
    virtual IVolume* Create() const;

    static void* operator new(unsigned long size) { return DWI_alloc(0, size, 1024); }

    unsigned char data[28];
};

IVolume* CVolSphere::Create() const {
    return new CVolSphere;
}
