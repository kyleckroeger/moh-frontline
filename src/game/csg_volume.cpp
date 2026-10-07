// CCSGVolume, the functions at the start of the file: an empty
// TransformedCopy, the extents as the union of the hierarchy object's
// sub-volume extents (starting from FLT_MAX and -FLT_MAX) and Create. The names come from
// the mangled symbols; IVolume's virtual functions are declared in the order
// of __vt__7IVolume, CCSGVolume declares its destructor first (defined
// elsewhere, so its virtual table is not emitted here) and its members are
// inferred. Create allocates a new CSG volume (12 bytes; the inline
// constructors set the virtual tables and clear the members). The rest of
// the file is not part of this unit.
class CMatrix;
class CDrawContext;
class CCollision;
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

class CHierObject {
public:
    IVolume* GetSubVolume(int) const;
};

class CCSGVolume : public IVolume {
public:
    virtual ~CCSGVolume();
    virtual IVolume* Create() const;
    virtual void TransformedCopy(const IVolume&, const CMatrix&);
    void GetExtents(CVector3&, CVector3&) const;

    CCSGVolume() : m_object(0), m_count(0) {}

    CHierObject* m_object;
    int m_count;
};

void CCSGVolume::TransformedCopy(const IVolume&, const CMatrix&) {
}

void CCSGVolume::GetExtents(CVector3& minimum, CVector3& maximum) const {
    minimum.x = 3.4028235e38f;
    minimum.y = 3.4028235e38f;
    minimum.z = 3.4028235e38f;
    maximum.x = -3.4028235e38f;
    maximum.y = -3.4028235e38f;
    maximum.z = -3.4028235e38f;
    for (int i = 0; i < m_count; i++) {
        IVolume* volume = m_object->GetSubVolume(i);
        if (volume) {
            CVector3 low;
            CVector3 high;
            volume->GetExtents(low, high);
            if (low.x < minimum.x)
                minimum.x = low.x;
            if (low.y < minimum.y)
                minimum.y = low.y;
            if (low.z < minimum.z)
                minimum.z = low.z;
            if (high.x > maximum.x)
                maximum.x = high.x;
            if (high.y > maximum.y)
                maximum.y = high.y;
            if (high.z > maximum.z)
                maximum.z = high.z;
        }
    }
}

IVolume* CCSGVolume::Create() const {
    return new CCSGVolume;
}
