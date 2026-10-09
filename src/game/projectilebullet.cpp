// CProjectileBullet: the weak bounding-volume, damage, firer and cast
// accessors. The class names come from the mangled symbols; the members and
// the weapon-record view are inferred from offsets, the result types are
// inferred, and the class is a non-virtual view. The accessors are inline in
// the original (weak symbols), so they are defined __declspec(weak). The static
// initialiser follows: the file's camera matrix (its inline constructor
// initialises the matrix class once) and the tracer render bin (built through
// the inline CRenderBinData/CRenderBin constructors with priority 7, its flag
// cleared, and its destructor registered). The rest of the file is not part
// of this unit. The bin's members, the 7 priority and the base layout are
// inferred; CProjectileBulletRenderBin declares Render (defined elsewhere)
// first so its global table stays elsewhere, and CRenderBin's inline
// destructor is a weak duplicate. The texture after the flag is opaque here.
class CDmaTag;
class CDmaPacket;

class CMatrix {
public:
    /* The default argument is not known; it does not change this code. */
    CMatrix(int unknown = 0) {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    static bool s_ClassInit;

    float m[4][4];
} __attribute__((aligned(16)));

class CRenderBinData {
public:
    CRenderBinData() : m_field0(0), m_field4(0), m_field8(0), m_fieldC(0), m_field10(0), m_priority(3) {}

    int m_field0;
    int m_field4;
    int m_field8;
    int m_fieldC;
    int m_field10;
    int m_priority;
    int m_field18;
};

class CRenderBin : public CRenderBinData {
public:
    virtual ~CRenderBin() {}
    virtual int Render(CDmaPacket&, void*);
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();
};

class CProjectileBulletRenderBin : public CRenderBin {
public:
    virtual int Render(CDmaPacket&, void*);
    CProjectileBulletRenderBin() : m_flag(false) { m_priority = 7; }
    virtual ~CProjectileBulletRenderBin();

    bool m_flag;
    unsigned char m_texture[64];
};

static CMatrix g_WorldToCamera;
static CProjectileBulletRenderBin g_BulletBin;

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
