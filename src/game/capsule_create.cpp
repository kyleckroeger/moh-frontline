// A fragment of capsule.cpp (0x800c7428): CVolCapsule::Create allocates a new,
// default-constructed capsule (80 bytes; the inline constructors set the
// IVolume and CVolCapsule virtual tables). IVolume and CVolCapsule are named
// by the mangled symbols; IVolume's virtual functions are declared in the
// order of __vt__7IVolume, CVolCapsule declares its destructor first (defined
// elsewhere, so its virtual table is not emitted here), and the capsule's
// data is an inferred block of the size the allocation shows. The rest of the
// file is not part of this unit.
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

class CVolCapsule : public IVolume {
public:
    virtual ~CVolCapsule();
    virtual IVolume* Create() const;

    unsigned char data[76];
};

IVolume* CVolCapsule::Create() const {
    return new CVolCapsule;
}
