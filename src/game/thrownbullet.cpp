// CThrownBullet: the weak bounding-volume, collision, light, damage and cast
// accessors. The class names come from the mangled symbols; the members and
// the weapon-record view are inferred from offsets, the result types are
// inferred, and the class is a non-virtual view. The accessors are inline in
// the original (weak symbols), so they are defined __declspec(weak). The
// velocity getter that follows copies a 16-byte vector as doublewords and the
// rest of the file is not part of this unit.
class CLight;

class ISceneNode {
public:
    enum EVolumeType {};
};

struct ThrownWeaponView {
    unsigned char unknown00[64];
    float damage;
};

class CThrownBullet {
public:
    const void* GetLocalBoundingVolume(ISceneNode::EVolumeType) const;
    const void* GetWorldBoundingVolume(ISceneNode::EVolumeType) const;
    bool IsCollisionEnabled() const;
    CLight* GetAttachedLight() const;
    float GetDamage() const;
    CThrownBullet* AsThrown();

    unsigned char unknown000[168];
    ThrownWeaponView* m_weapon;
    unsigned char unknown0ac[164];
    unsigned char m_localVolume[80];
    unsigned char m_worldVolume[80];
    CLight* m_attachedLight;
};

__declspec(weak) const void* CThrownBullet::GetLocalBoundingVolume(ISceneNode::EVolumeType) const {
    return m_localVolume;
}

__declspec(weak) const void* CThrownBullet::GetWorldBoundingVolume(ISceneNode::EVolumeType) const {
    return m_worldVolume;
}

__declspec(weak) bool CThrownBullet::IsCollisionEnabled() const {
    return true;
}

__declspec(weak) CLight* CThrownBullet::GetAttachedLight() const {
    return m_attachedLight;
}

__declspec(weak) float CThrownBullet::GetDamage() const {
    return m_weapon->damage;
}

__declspec(weak) CThrownBullet* CThrownBullet::AsThrown() {
    return this;
}
