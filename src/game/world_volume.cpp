// CWorldVolume: the weak TestCollision defaults (no collision) for triangles,
// planes, points, world volumes and CDB objects. The names come from the
// mangled symbols; the class is a non-virtual view and the result type (bool)
// is inferred. They are inline in the original (weak symbols), so they are
// defined __declspec(weak). The rest of the file is not part of this unit.
class CCollision;
class CTriangle;
class CPlane;
class CCDBObject;

class CVector3 {
public:
    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CWorldVolume {
public:
    bool TestCollision(const CTriangle&, CCollision&, bool) const;
    bool TestCollision(const CPlane&, CCollision&, bool) const;
    bool TestCollision(CVector3, CCollision&, bool) const;
    bool TestCollision(const CWorldVolume&, CCollision&, bool) const;
    bool TestCollision(const CCDBObject&, CCollision&, bool) const;
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
