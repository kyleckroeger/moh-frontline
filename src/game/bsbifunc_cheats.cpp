// A fragment of bsbifunc.cpp (0x80026c78): script built-ins that eject a shell
// from the calling node's player's current weapon (a shell CRC chosen by kind 1
// to 5, kind 1's for others; not when the player's no-shell flag is set),
// return whether the invincibility cheat is on (the first byte of g_cheats) and
// set the first player's super-aim mode (a flag bit of the player). Each reads
// its argument below the script stack top and pops the built-in's arguments.
// The file name is this project's; the original record is bsbifunc.cpp and the
// built-ins around these are not reconstructed. The functions, classes and
// globals are named by the mangled symbols; ISceneNode is declared with its
// virtual functions in the order of __vt__10ISceneNode (AsPlayerObject at
// +156); the cheats (20 bytes, the size of g_cheats), player, scene and
// built-in record views are inferred (members at their offsets, names not
// original).
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


struct CheatsView {
    bool invincible;
    unsigned char unknown01[19];
};

class CPlayerWeaponObject {
public:
    void CreateEjectedShell(int);
};

class CPlayerObject {
public:
    CPlayerWeaponObject* GetCurrentPlayerWeapon() const;

    unsigned char unknown000[918];
    unsigned char unknown396 : 1;
    unsigned char m_noShells : 1;
    unsigned char unknown396b : 6;
    unsigned char unknown397;
    unsigned char unknown398 : 1;
    unsigned char m_superAim : 1;
    unsigned char unknown398b : 6;
};

class CScene {
public:
    CPlayerObject* GetPlayer(int) const;

    unsigned char unknown000[328];
};

extern CheatsView g_cheats;
extern CScene g_scene;

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

void BIFunc_PlayerEjectShell(int** stack, void* object) {
    int kind = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    CPlayerObject* player = ((ISceneNode*)object)->AsPlayerObject();
    CPlayerWeaponObject* weapon = player->GetCurrentPlayerWeapon();
    int shell;
    switch (kind) {
    case 1:
        shell = 0x997035a9;
        break;
    case 2:
        shell = 0x37ab0f02;
        break;
    case 3:
        shell = 0x9ab689e6;
        break;
    case 4:
        shell = 0x4375a62a;
        break;
    case 5:
        shell = 0xb2134d62;
        break;
    default:
        shell = 0x997035a9;
        break;
    }
    if (weapon && !player->m_noShells)
        weapon->CreateEjectedShell(shell);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_IsPlayerInvincible(int** stack, void*) {
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = g_cheats.invincible;
}

void BIFunc_PlayerSuperAimMode(int** stack, void*) {
    bool enable = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1)) != 0;
    g_scene.GetPlayer(0)->m_superAim = enable;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
