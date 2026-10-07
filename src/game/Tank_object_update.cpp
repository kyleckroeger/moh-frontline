// CTankObject's update, clean-up and construction (a fragment of
// Tank_object.cpp, from CommitUpdate to VerifyAIDataSize): the update hooks
// forward to CHierObject, update the weapons and the AI doodad, and every 60
// ticks re-enable the A* path nodes and disable those colliding with the
// tank's world volume; the destructor and the two constructors set up the
// doodad, two bounding spheres, the weapon slot map (-1) and the weapons.
// The names come from the mangled symbols and the vtable order from
// __vt__11CTankObject (slots through GetWorldBoundingVolume); IVolume's first
// virtual is a placeholder. CTankObject declares Draw first so this file does
// not emit its vtable. The members are inferred from offsets (the doodad's
// size fills the space to the look sub-object at +1940; the flag at +480 is
// the byte's last bit field), and the global objects have their symbols'
// sizes.
class CDrawContext;
class CCollision;
class IVolume;
class CTankObject;

class ISceneNode {
public:
    enum EVolumeType {};

    virtual void MarkForDestruction(int);
    virtual ~ISceneNode();
    virtual void Destroy();
    virtual void BeginUpdate(float);
    virtual void UpdateAI(float);
    virtual void CommitAI();
    virtual void ConstrainVelocity();
    virtual void AttemptUpdate(float);
    virtual void OnCollision(const CCollision&);
    virtual void CommitUpdate();
    virtual void Draw(CDrawContext&);
    virtual IVolume* GetLocalBoundingVolume(EVolumeType) const;
    virtual IVolume* GetWorldBoundingVolume(EVolumeType) const;
};

class CStaticMesh;

class CStaticObject : public ISceneNode {
public:
    struct MotionFrame;
};

class CHierObject : public CStaticObject {
public:
    CHierObject(CStaticMesh*, int, int, int, int, int, CStaticObject::MotionFrame*);
    CHierObject();
    virtual ~CHierObject();
    void BeginUpdate(float);
    void AttemptUpdate(float);
    void CommitUpdate();
    void CleanUp();
};

class CWeapon {
public:
    void Update(float, bool);
};

class CWeaponFactory {
public:
    void DestroyWeapon(CWeapon*);

    unsigned char unknown00[28];
};

class CAIFilterGlobal {
public:
    void EnableAllAStarPathNodes();
    void DisableCollidingAStarPathNodes(const IVolume*);

    unsigned char unknown00[24];
};

class IVolume {
public:
    virtual void Unknown08();
};

class CVolSphere : public IVolume {
public:
    virtual ~CVolSphere();

    unsigned char unknown04[28];
};

class CAITankDoodad {
public:
    CAITankDoodad();
    ~CAITankDoodad();
    void Update(float);
    void PreUpdate();
    void CleanUp();
    void Init(CTankObject*);

    unsigned char unknown00[84];
};

extern CWeaponFactory g_WeaponFactory;
extern CAIFilterGlobal g_aigAIFilterGlobalObject;

extern "C" void* memset(void*, int, unsigned long);

class CTankObject : public CHierObject {
public:
    void Draw(CDrawContext&);
    virtual ~CTankObject();
    CTankObject(CStaticMesh*, int, int, int, int, int, CStaticObject::MotionFrame*);
    CTankObject();
    void CommitUpdate();
    void AttemptUpdate(float);
    void UpdateAI(float);
    void BeginUpdate(float);
    void CleanUp();
    void InitAI();
    void VerifyAIDataSize();

    unsigned char unknown004[476];
    unsigned char unknown1E0b80 : 7;
    unsigned char m_flag1E0 : 1;
    unsigned char unknown1E1[1375];
    CAITankDoodad m_aiDoodad;
    int m_lookSubObject;
    int m_pathNodeTimer;
    int m_weaponSlot[8];
    CWeapon* m_weapons[3];
    unsigned char unknown7C8[8];
    CVolSphere m_sphere0;
    CVolSphere m_sphere1;
};

void CTankObject::CommitUpdate() {
    CHierObject::CommitUpdate();
}

void CTankObject::AttemptUpdate(float dt) {
    CHierObject::AttemptUpdate(dt);
}

void CTankObject::UpdateAI(float dt) {
    m_aiDoodad.Update(dt);
    if (m_pathNodeTimer <= 0) {
        g_aigAIFilterGlobalObject.EnableAllAStarPathNodes();
        g_aigAIFilterGlobalObject.DisableCollidingAStarPathNodes(GetWorldBoundingVolume((EVolumeType)0));
        m_pathNodeTimer = 60;
    } else {
        m_pathNodeTimer -= (int)dt;
    }
}

void CTankObject::BeginUpdate(float dt) {
    int i;

    CHierObject::BeginUpdate(dt);
    for (i = 0; i < 3; i++) {
        if (m_weapons[i])
            m_weapons[i]->Update(dt, false);
    }
    m_aiDoodad.PreUpdate();
}

CTankObject::~CTankObject() {
}

void CTankObject::CleanUp() {
    int i;

    g_aigAIFilterGlobalObject.EnableAllAStarPathNodes();
    m_aiDoodad.CleanUp();
    for (i = 0; i < 3; i++) {
        if (m_weapons[i])
            g_WeaponFactory.DestroyWeapon(m_weapons[i]);
    }
    CHierObject::CleanUp();
}

void CTankObject::InitAI() {
    m_aiDoodad.Init(this);
}

CTankObject::CTankObject(CStaticMesh* mesh, int a, int b, int c, int d, int e, CStaticObject::MotionFrame* frames)
    : CHierObject(mesh, a, b, c, d, e, frames) {
    memset(m_weaponSlot, -1, sizeof(m_weaponSlot));
    memset(m_weapons, 0, sizeof(m_weapons));
    m_lookSubObject = 0;
    m_pathNodeTimer = 0;
    m_flag1E0 = 1;
}

CTankObject::CTankObject() {
    memset(m_weaponSlot, -1, sizeof(m_weaponSlot));
    memset(m_weapons, 0, sizeof(m_weapons));
    m_lookSubObject = 0;
    m_pathNodeTimer = 0;
    m_flag1E0 = 1;
}

void CTankObject::VerifyAIDataSize() {
}
