// CAnimatedPlayer's bounding-volume getters and flocking parameters: the
// world and local volumes are chosen by volume type (types 0 to 2 share one
// volume, type 3 has its own, others have none), and the flocking parameters
// are a float and a long stored together. CAnimatedPlayer, ISceneNode and
// EVolumeType are named by the mangled symbols; the members are inferred
// from offsets, the volumes are opaque IVolume pointers, and the class is a
// non-virtual view (its virtual table is emitted with the rest of the file).
class IVolume;

class ISceneNode {
public:
    enum EVolumeType {};
};

class CAnimatedPlayer {
public:
    const IVolume* GetWorldBoundingVolume(ISceneNode::EVolumeType) const;
    const IVolume* GetLocalBoundingVolume(ISceneNode::EVolumeType) const;
    void GetFlocoParams(float&, long&);
    void SetFlocoParams(float, long);

    unsigned char unknown0000[9088];
    IVolume* m_localVolume3;
    IVolume* m_localVolume;
    IVolume* m_worldVolume3;
    IVolume* m_worldVolume;
    unsigned char unknown2390[8484];
    float m_flocoParam;
    long m_flocoCount;
};

const IVolume* CAnimatedPlayer::GetWorldBoundingVolume(ISceneNode::EVolumeType type) const {
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

const IVolume* CAnimatedPlayer::GetLocalBoundingVolume(ISceneNode::EVolumeType type) const {
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

void CAnimatedPlayer::GetFlocoParams(float& param, long& count) {
    param = m_flocoParam;
    count = m_flocoCount;
}

void CAnimatedPlayer::SetFlocoParams(float param, long count) {
    m_flocoParam = param;
    m_flocoCount = count;
}
