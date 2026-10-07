// A fragment of static_obj.cpp (0x800b1bd0): CStaticObject's world and local
// bounding-volume getters: the volume pointers kept for type 3 and for types
// 0 to 2, none for other types. CStaticObject, ISceneNode, IVolume and
// EVolumeType are named by the mangled symbols; the members are inferred and
// CStaticObject is a non-virtual view. SetWorldBoundingVolume after these and
// the rest of the file are not part of this unit.
class IVolume;

class ISceneNode {
public:
    enum EVolumeType {};
};

class CStaticObject {
public:
    IVolume* GetWorldBoundingVolume(ISceneNode::EVolumeType) const;
    IVolume* GetLocalBoundingVolume(ISceneNode::EVolumeType) const;

    unsigned char unknown000[264];
    IVolume* m_localVolume3;
    IVolume* m_localVolume;
    IVolume* m_worldVolume3;
    IVolume* m_worldVolume;
};

IVolume* CStaticObject::GetWorldBoundingVolume(ISceneNode::EVolumeType type) const {
    switch (type) {
    case 3:
        return m_worldVolume3;
    case 0:
    case 1:
    case 2:
        return m_worldVolume;
    case 4:
        return 0;
    }
    return 0;
}

IVolume* CStaticObject::GetLocalBoundingVolume(ISceneNode::EVolumeType type) const {
    switch (type) {
    case 3:
        return m_localVolume3;
    case 0:
    case 1:
    case 2:
        return m_localVolume;
    case 4:
        return 0;
    }
    return 0;
}
