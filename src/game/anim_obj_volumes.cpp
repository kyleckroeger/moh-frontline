// A fragment of anim_obj.cpp (0x800950d8): CBullet's weak defaults emitted in
// this file (no script bullet type, not thrown, not a projectile), then
// CAnimObject::TriggerScriptEvent (forwarded to the script object when there
// is one) and the bounding-volume getters and setters, which keep two volumes
// each for the world and local spaces (type 3 selects the first, types 0 to 2
// the second; the setters store only types 3 and 0, with an empty case 4). The file name is this
// project's; the original record is anim_obj.cpp; GetDamage before these uses
// a pooled .sdata2 constant and Draw after them is not reconstructed. The
// classes and functions are named by the mangled symbols; CBullet and
// CAnimObject are inferred non-virtual views (members at their offsets, names
// not original), and the result types are inferred.
class IVolume;
class CThrownBullet;
class CProjectileBullet;
struct BSObject;

void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);

class ISceneNode {
public:
    enum EVolumeType {};
};

class CBullet {
public:
    int GetScriptBulletType() const;
    CThrownBullet* AsThrown();
    CProjectileBullet* AsProjectile();
};

class CAnimObject {
public:
    void TriggerScriptEvent(int, void*, bool);
    IVolume* GetWorldBoundingVolume(ISceneNode::EVolumeType) const;
    IVolume* GetLocalBoundingVolume(ISceneNode::EVolumeType) const;
    void SetWorldBoundingVolume(ISceneNode::EVolumeType, IVolume*);
    void SetLocalBoundingVolume(ISceneNode::EVolumeType, IVolume*);

    unsigned char unknown0000[9088];
    IVolume* m_localVolume3;
    IVolume* m_localVolume;
    IVolume* m_worldVolume3;
    IVolume* m_worldVolume;
    BSObject* m_script;
};

__declspec(weak) int CBullet::GetScriptBulletType() const {
    return -1;
}

__declspec(weak) CThrownBullet* CBullet::AsThrown() {
    return 0;
}

__declspec(weak) CProjectileBullet* CBullet::AsProjectile() {
    return 0;
}

void CAnimObject::TriggerScriptEvent(int event, void* data, bool flag) {
    if (m_script)
        BSObjectTriggerEvent(m_script, event, data, 0, flag);
}

IVolume* CAnimObject::GetWorldBoundingVolume(ISceneNode::EVolumeType type) const {
    switch (type) {
    case 3:
        return m_worldVolume3;
    case 0:
    case 1:
    case 2:
        return m_worldVolume;
    case 4:
        return 0;
    default:
        return 0;
    }
}

IVolume* CAnimObject::GetLocalBoundingVolume(ISceneNode::EVolumeType type) const {
    switch (type) {
    case 3:
        return m_localVolume3;
    case 0:
    case 1:
    case 2:
        return m_localVolume;
    case 4:
        return 0;
    default:
        return 0;
    }
}

void CAnimObject::SetWorldBoundingVolume(ISceneNode::EVolumeType type, IVolume* volume) {
    switch (type) {
    case 4:
        break;
    case 3:
        m_worldVolume3 = volume;
        break;
    case 0:
        m_worldVolume = volume;
        break;
    }
}

void CAnimObject::SetLocalBoundingVolume(ISceneNode::EVolumeType type, IVolume* volume) {
    switch (type) {
    case 4:
        break;
    case 3:
        m_localVolume3 = volume;
        break;
    case 0:
        m_localVolume = volume;
        break;
    }
}
