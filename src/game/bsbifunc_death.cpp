// A fragment of bsbifunc.cpp (0x80030348): built-ins that return the
// player's weapon refire rate (0 without a weapon), start the player's death
// sequence (in multiplayer also dropping the multiplayer dump object; in
// single player for the first player) and stop the first player's path. Each
// pops the built-in's arguments. The file name is this project's; the
// original record is bsbifunc.cpp and the built-ins around these are not
// reconstructed. The functions, classes and globals are named by the mangled
// symbols; ISceneNode is declared with its virtual functions in the order of
// __vt__10ISceneNode, and the weapon, scene and built-in record views are
// inferred.
enum EClsnId {};
class CCollision;
class CDrawContext;
class CMatrix;
class CVector3;
class CBullet;
class CLight;
class CPlayerObject;
struct AIDoodadView;

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
    virtual int GetLocalBoundingVolume(EVolumeType) const;
    virtual int GetWorldBoundingVolume(EVolumeType) const;
    virtual void GetTMLocalToWorld(CMatrix&) const;
    virtual void GetPosition(CVector3&) const;
    virtual void GetRightward(CVector3&) const;
    virtual void GetForward(CVector3&) const;
    virtual void GetUpward(CVector3&) const;
    virtual int IsVisible(CDrawContext&) const;
    virtual int IsDrawEnabled() const;
    virtual EClsnId GetCollisionId() const;
    virtual void SetCollisionId(EClsnId);
    virtual int GetScriptObject() const;
    virtual void TriggerScriptEvent(int, void*, bool);
    virtual void HandleBulletCollision(CBullet*, const CCollision&);
    virtual void* AsMovingNode();
    virtual const void* AsMovingNode() const;
    virtual void* AsStaticObject();
    virtual const void* AsStaticObject() const;
    virtual void* AsHierObject();
    virtual const void* AsHierObject() const;
    virtual void* AsWorldObject();
    virtual const void* AsWorldObject() const;
    virtual void* AsAnimObject();
    virtual const void* AsAnimObject() const;
    virtual void* AsSoldierObject();
    virtual const void* AsSoldierObject() const;
    virtual CPlayerObject* AsPlayerObject();
    virtual const void* AsPlayerObject() const;
    virtual void* AsPlayerWeaponObject();
    virtual const void* AsPlayerWeaponObject() const;
    virtual void* AsAnimatedPlayerObject();
    virtual const void* AsAnimatedPlayerObject() const;
    virtual CLight* AsLight();
    virtual const CLight* AsLight() const;
    virtual CBullet* AsBullet();
    virtual const CBullet* AsBullet() const;
    virtual void* AsCollisionVolume();
    virtual const void* AsCollisionVolume() const;
    virtual AIDoodadView* GetAIDoodad();
    virtual const void* GetAIDoodad() const;
    virtual void SetAttachedLight(CLight*, CVector3);
    virtual CLight* GetAttachedLight() const;
};

struct WeaponPropertiesView {
    unsigned char unknown0[6];
    short refireRate;
};

class CWeapon {
public:
    unsigned char unknown000[640];
    WeaponPropertiesView* m_properties;
};

class CPlayerObject {
public:
    CWeapon* GetCurrentWeapon() const;
    void StartDeathSequence();
    void DropMPDumpObject();
    void StopPath();
};

class CScene {
public:
    CPlayerObject* GetPlayer(int) const;

    unsigned char unknown000[328];
};

extern CScene g_scene;
extern bool g_bInMultiplayerMode;

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

void BIFunc_PlayerGetWeaponRefireRate(int** stack, void* object) {
    CWeapon* weapon = ((ISceneNode*)object)->AsPlayerObject()->GetCurrentWeapon();
    int rate = 0;
    if (weapon)
        rate = weapon->m_properties->refireRate;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = rate;
}

void BIFunc_PlayerDeath(int** stack, void* object) {
    CPlayerObject* player = ((ISceneNode*)object)->AsPlayerObject();
    if (g_bInMultiplayerMode) {
        player->StartDeathSequence();
        player->DropMPDumpObject();
    } else {
        g_scene.GetPlayer(0)->StartDeathSequence();
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_PlayerStopPath(int** stack, void*) {
    g_scene.GetPlayer(0)->StopPath();
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
