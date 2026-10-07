// A fragment of Soldier_object.cpp (0x800ac310): CSoldierObject's world and
// local bounding-volume getters: volume pointers for types 3 and 0, embedded
// volumes for types 1 and 2, none for other types. CSoldierObject,
// ISceneNode, IVolume and EVolumeType are named by the mangled symbols; the
// members are inferred and CSoldierObject is a non-virtual view.
// UpdateLocalBoundingVolume after these and the rest of the file are not part
// of this unit.
class IVolume;

/* inferred: storage of an embedded volume object (its class is not known) */
struct EmbeddedVolumeView {
    unsigned char data[32];
};

class ISceneNode {
public:
    enum EVolumeType {};
};

class CSoldierObject {
public:
    IVolume* GetWorldBoundingVolume(ISceneNode::EVolumeType) const;
    IVolume* GetLocalBoundingVolume(ISceneNode::EVolumeType) const;

    unsigned char unknown0000[9088];
    IVolume* m_localVolume3;
    IVolume* m_localVolume0;
    IVolume* m_worldVolume3;
    IVolume* m_worldVolume0;
    unsigned char unknown2390[8080];
    EmbeddedVolumeView m_localVolume1;
    EmbeddedVolumeView m_worldVolume1;
    EmbeddedVolumeView m_localVolume2;
    EmbeddedVolumeView m_worldVolume2;
};

IVolume* CSoldierObject::GetWorldBoundingVolume(ISceneNode::EVolumeType type) const {
    switch (type) {
    case 3:
        return m_worldVolume3;
    case 0:
        return m_worldVolume0;
    case 1:
        return (IVolume*)&m_worldVolume1;
    case 2:
        return (IVolume*)&m_worldVolume2;
    case 4:
        return 0;
    }
    return 0;
}

IVolume* CSoldierObject::GetLocalBoundingVolume(ISceneNode::EVolumeType type) const {
    switch (type) {
    case 3:
        return m_localVolume3;
    case 0:
        return m_localVolume0;
    case 1:
        return (IVolume*)&m_localVolume1;
    case 2:
        return (IVolume*)&m_localVolume2;
    case 4:
        return 0;
    }
    return 0;
}
