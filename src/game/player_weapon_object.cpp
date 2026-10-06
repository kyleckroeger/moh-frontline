// CPlayerWeaponObject, the functions at the start of the file: initialisation,
// identity casts, visibility (the draw-enabled virtual), the sleeve texture
// from the level's texture pack, destruction through the animated-object
// factory, the player's muzzle flag and the weapon name. The class names come
// from the mangled symbols. CPlayerWeaponObject is an inferred, non-virtual
// view whose CAnimObject base (at the start of the object) is reached through
// casts; CAnimObject is declared with the ISceneNode virtual functions in the
// order of __vt__10ISceneNode, flattened. Only the members these functions
// touch are declared, at their offsets (their names are not original). The
// weapon's flag byte is an inferred bit-field view. CreateEjectedShell and the
// rest of the file are not reconstructed.
extern "C" char* strcpy(char*, const char*);

class CDrawContext;
class CCollision;
class CMatrix;
class CVector3;
class CTexture;
struct LevelFileContentsStruct_;
enum TLTResourceID {};

LevelFileContentsStruct_* TLT_FindNextResourceByType(TLTResourceID, LevelFileContentsStruct_*);
void* TLT_LoadFromRepository(const char*, int*, bool*);

struct LevelResourceView {
    unsigned char unknown00[228];
    const char* name;
};

struct LevelFileContentsView {
    unsigned char unknown00[8];
    LevelResourceView* resource;
};

class CTexturePack {
public:
    CTexture* FindTexture(char*);
};

class CWeaponFlagsView {
public:
    unsigned char unknown000[704];
    unsigned char flag0 : 1;
    unsigned char flag1 : 1;
    unsigned char flag2 : 1;
    unsigned char flag3 : 1;
    unsigned char flag4 : 1;
    unsigned char muzzle : 1;
    unsigned char flag6 : 1;
    unsigned char flag7 : 1;
};

class CPlayerObject {
public:
    CWeaponFlagsView* GetCurrentWeapon() const;
};

class CScene {
public:
    CPlayerObject* GetPlayer(int) const;

    char data[328];
};

extern CScene g_scene;

struct BSObject;

class CAnimObject {
public:
    enum EVolumeType {};

    virtual void MarkForDestruction(int);
    virtual ~CAnimObject();
    virtual void Destroy();
    virtual void BeginUpdate(float);
    virtual void UpdateAI(float);
    virtual void CommitAI();
    virtual void ConstrainVelocity();
    virtual void AttemptUpdate(float);
    virtual void OnCollision(const CCollision&);
    virtual void CommitUpdate();
    virtual void Draw(CDrawContext&);
    virtual const void* GetLocalBoundingVolume(EVolumeType) const;
    virtual const void* GetWorldBoundingVolume(EVolumeType) const;
    virtual void GetTMLocalToWorld(CMatrix&) const;
    virtual void GetPosition(CVector3&) const;
    virtual void GetRightward(CVector3&) const;
    virtual void GetForward(CVector3&) const;
    virtual void GetUpward(CVector3&) const;
    virtual bool IsVisible(CDrawContext&) const;
    virtual bool IsDrawEnabled() const;

    void Init();
};

class CAnimObjectFactory {
public:
    void ReleaseObject(CAnimObject*);

    unsigned char data[16];
};

extern CAnimObjectFactory g_AnimObjFactory;

class CPlayerWeaponObject {
public:
    void Init();
    const CPlayerWeaponObject* AsPlayerWeaponObject() const;
    CPlayerWeaponObject* AsPlayerWeaponObject();
    BSObject* GetScriptObject() const;
    bool IsVisible(CDrawContext&) const;
    void SetSleeveTexture(char*);
    void Destroy();
    static void InitClass();
    void SetWeaponName(const char*);

    unsigned char unknown0000[12724];
    char m_weaponName[36];
    CTexture* m_sleeveTexture;
};

__declspec(weak) void CPlayerWeaponObject::Init() {
    // The CAnimObject base is at the start of the object.
    ((CAnimObject*)this)->Init();
}

__declspec(weak) const CPlayerWeaponObject* CPlayerWeaponObject::AsPlayerWeaponObject() const {
    return this;
}

__declspec(weak) CPlayerWeaponObject* CPlayerWeaponObject::AsPlayerWeaponObject() {
    return this;
}

__declspec(weak) BSObject* CPlayerWeaponObject::GetScriptObject() const {
    return 0;
}

bool CPlayerWeaponObject::IsVisible(CDrawContext&) const {
    return ((const CAnimObject*)this)->IsDrawEnabled();
}

void CPlayerWeaponObject::SetSleeveTexture(char* name) {
    bool loaded = false;
    LevelFileContentsView* contents = (LevelFileContentsView*)TLT_FindNextResourceByType((TLTResourceID)13, 0);
    CTexturePack* pack = (CTexturePack*)TLT_LoadFromRepository(contents->resource->name, 0, &loaded);
    m_sleeveTexture = pack->FindTexture(name);
}

void CPlayerWeaponObject::Destroy() {
    g_AnimObjFactory.ReleaseObject((CAnimObject*)this);
}

void CPlayerWeaponObject::InitClass() {
}

void SetMuzzleStatusForPlayer(bool) {
    CWeaponFlagsView* weapon = g_scene.GetPlayer(0)->GetCurrentWeapon();
    weapon->muzzle = true;
}

void CPlayerWeaponObject::SetWeaponName(const char* name) {
    if (name)
        strcpy(m_weaponName, name);
    else
        m_weaponName[0] = 0;
}
