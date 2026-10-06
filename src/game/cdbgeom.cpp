// CCDBVolBox collision tests emitted in cdbgeom.cpp: no collision with world
// volumes, and the point, CDB-object, box and generic-volume tests made as its
// CVolBox base. The names come from the mangled symbols; CVolBox's tests are a
// non-virtual view of the base and the result type (bool) is inferred. They
// are inline in the original (weak symbols), so they are defined
// __declspec(weak). The destructor before them and the rest of the file are
// not part of this unit.
class CCollision;
class CWorldVolume;
class CCDBObject;
class IVolume;

class CVector3 {
public:
    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CVolBox {
public:
    bool TestCollision(CVector3, CCollision&, bool) const;
    bool TestCollision(const CCDBObject&, CCollision&, bool) const;
    bool TestCollision(const CVolBox&, CCollision&, bool) const;
    bool TestCollision(const IVolume&, CCollision&, bool) const;
};

class CCDBVolBox : public CVolBox {
public:
    bool TestCollision(const CWorldVolume&, CCollision&, bool) const;
    bool TestCollision(CVector3, CCollision&, bool) const;
    bool TestCollision(const CCDBObject&, CCollision&, bool) const;
    bool TestCollision(const CVolBox&, CCollision&, bool) const;
    bool TestCollision(const IVolume&, CCollision&, bool) const;
};

__declspec(weak) bool CCDBVolBox::TestCollision(const CWorldVolume&, CCollision&, bool) const {
    return false;
}

__declspec(weak) bool CCDBVolBox::TestCollision(CVector3 point, CCollision& collision, bool flag) const {
    return CVolBox::TestCollision(point, collision, flag);
}

__declspec(weak) bool CCDBVolBox::TestCollision(const CCDBObject& other, CCollision& collision, bool flag) const {
    return CVolBox::TestCollision(other, collision, flag);
}

__declspec(weak) bool CCDBVolBox::TestCollision(const CVolBox& other, CCollision& collision, bool flag) const {
    return CVolBox::TestCollision(other, collision, flag);
}

__declspec(weak) bool CCDBVolBox::TestCollision(const IVolume& other, CCollision& collision, bool flag) const {
    return CVolBox::TestCollision(other, collision, flag);
}
