// CThrownBullet: the weak bounding-volume, collision, light, damage and cast
// accessors. The class names come from the mangled symbols; the members and
// the weapon-record view are inferred from offsets, the result types are
// inferred, and the class is a non-virtual view. The accessors are inline in
// the original (weak symbols), so they are defined __declspec(weak). The
// weak velocity getter and the light-volume forwards (to the manager at +212)
// follow; SomebodyCaughtYou (a virtual call) and the rest of the file are not
// part of this unit.
class CLight;
struct BPDLightVolume;

// CVector3 view: four floats, 8-byte aligned, overlaid with two doubles. The
// union is inferred from the code, not the original declaration: the velocity
// is copied as two lfd/stfd pairs, which an implicit copy through the double
// pair reproduces.
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

class CLightVolumeManager {
public:
    void* GetVolume();
    void RemoveVolume(BPDLightVolume*);
    void AddVolume(BPDLightVolume*);

    unsigned char unknown00[28];
};

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
    void GetVelocity(CVector3&);
    void* GetLightVolume();
    void ExitLightVolume(BPDLightVolume*);
    void EnterLightVolume(BPDLightVolume*);

    unsigned char unknown000[168];
    ThrownWeaponView* m_weapon;
    unsigned char unknown0ac[40];
    CLightVolumeManager m_lightVolumes;
    CVector3 m_velocity;
    unsigned char unknown100[80];
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

__declspec(weak) void CThrownBullet::GetVelocity(CVector3& v) {
    v = m_velocity;
}

void* CThrownBullet::GetLightVolume() {
    return m_lightVolumes.GetVolume();
}

void CThrownBullet::ExitLightVolume(BPDLightVolume* volume) {
    m_lightVolumes.RemoveVolume(volume);
}

void CThrownBullet::EnterLightVolume(BPDLightVolume* volume) {
    m_lightVolumes.AddVolume(volume);
}
