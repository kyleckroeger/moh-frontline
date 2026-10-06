// CProjectileBullet: the weak bounding-volume, damage, firer and cast
// accessors. The class names come from the mangled symbols; the members and
// the weapon-record view are inferred from offsets, the result types are
// inferred, and the class is a non-virtual view. The accessors are inline in
// the original (weak symbols), so they are defined __declspec(weak). The static
// initialiser that follows and the rest of the file are not part of this unit.
class ISceneNode {
public:
    enum EVolumeType {};
};

struct ProjectileWeaponView {
    unsigned char unknown00[48];
    float damage;
};

class CProjectileBullet {
public:
    const void* GetLocalBoundingVolume(ISceneNode::EVolumeType) const;
    const void* GetWorldBoundingVolume(ISceneNode::EVolumeType) const;
    float GetDamage() const;
    ISceneNode* GetFiredBy() const;
    CProjectileBullet* AsProjectile();

    unsigned char unknown000[168];
    ProjectileWeaponView* m_weapon;
    ISceneNode* m_firedBy;
    unsigned char unknown0b0[16];
    unsigned char m_localVolume[80];
    unsigned char m_worldVolume[80];
};

__declspec(weak) const void* CProjectileBullet::GetLocalBoundingVolume(ISceneNode::EVolumeType) const {
    return m_localVolume;
}

__declspec(weak) const void* CProjectileBullet::GetWorldBoundingVolume(ISceneNode::EVolumeType) const {
    return m_worldVolume;
}

__declspec(weak) float CProjectileBullet::GetDamage() const {
    return m_weapon->damage;
}

__declspec(weak) ISceneNode* CProjectileBullet::GetFiredBy() const {
    return m_firedBy;
}

__declspec(weak) CProjectileBullet* CProjectileBullet::AsProjectile() {
    return this;
}
