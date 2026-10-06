// A fragment of collisionvolume.cpp (0x80098134): CCollisionVolume's bounding
// volume getters (both return the volume pointer), the row accessors
// GetUpward, GetForward, GetRightward and GetPosition (each copies one row of
// the transform into the result) and GetTMLocalToWorld (copies the whole
// transform). The file name is this project's; the original record is
// collisionvolume.cpp (collisionvolume.cpp holds the start of the file) and
// the bullet handler and BeginUpdate before these are not reconstructed. The
// class names come from the mangled symbols; CCollisionVolume is an inferred
// non-virtual view (members at their offsets, names not original), and the
// row getters returning by value are inferred inline helpers.
//
// CVector3 view: four floats, 8-byte aligned, overlaid with two doubles. The
// union is inferred from the code, not the original declaration: the row
// copies move each vector as two lfd/stfd pairs, which an implicit copy
// through the double pair reproduces.
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
    CMatrix& operator=(const CMatrix&);

    CVector3 right;
    CVector3 forward;
    CVector3 up;
    CVector3 position;
};

class IVolume;

class ISceneNode {
public:
    enum EVolumeType {};
};

class CCollisionVolume {
public:
    IVolume* GetLocalBoundingVolume(ISceneNode::EVolumeType) const;
    IVolume* GetWorldBoundingVolume(ISceneNode::EVolumeType) const;
    void GetUpward(CVector3&) const;
    void GetForward(CVector3&) const;
    void GetRightward(CVector3&) const;
    void GetPosition(CVector3&) const;
    void GetTMLocalToWorld(CMatrix&) const;

    CVector3 GetUp() const { return m_tm.up; }
    CVector3 GetFwd() const { return m_tm.forward; }
    CVector3 GetRight() const { return m_tm.right; }
    CVector3 GetPos() const { return m_tm.position; }

    unsigned char unknown00[48];
    CMatrix m_tm;
    IVolume* m_volume;
};

IVolume* CCollisionVolume::GetLocalBoundingVolume(ISceneNode::EVolumeType) const {
    return m_volume;
}

IVolume* CCollisionVolume::GetWorldBoundingVolume(ISceneNode::EVolumeType) const {
    return m_volume;
}

void CCollisionVolume::GetUpward(CVector3& v) const {
    v = GetUp();
}

void CCollisionVolume::GetForward(CVector3& v) const {
    v = GetFwd();
}

void CCollisionVolume::GetRightward(CVector3& v) const {
    v = GetRight();
}

void CCollisionVolume::GetPosition(CVector3& v) const {
    v = GetPos();
}

void CCollisionVolume::GetTMLocalToWorld(CMatrix& matrix) const {
    matrix = m_tm;
}
