// A fragment of bsbifunc.cpp (0x800307dc): player built-ins that start a
// motion shake, allow or forbid crouching (a flag bit of the player) and stop a
// camera shake (on the script object's player in multiplayer, otherwise the
// first player). Each reads its arguments below the script stack top and pops
// the built-in's arguments. The file name is this project's; the original
// record is bsbifunc.cpp; PlayerScreenFlash before these uses pooled constants
// and is not reconstructed. The functions, classes and globals are named by the
// mangled symbols; ISceneNode is declared with its virtual functions in the
// order of __vt__10ISceneNode (AsPlayerObject at +156) and BSGO_Basic in the
// order of __vt__10BSGO_Basic; the player, scene and built-in record views are
// inferred (members at their offsets, names not original).
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


class BSGO_Basic {
    unsigned char unknown00[12];

public:
    virtual void Destroy();
    virtual int GetScriptData();
    virtual ISceneNode* GetSceneNode();
};

struct BSObjectView {
    unsigned char unknown00[12];
    BSGO_Basic* user;
};

class CPlayerObject {
public:
    void DoMotionShake(float, float);
    void StopCameraShake(float);

    unsigned char unknown000[919];
    unsigned char unknown397 : 4;
    unsigned char m_canCrouch : 1;
    unsigned char unknown397b : 3;
};

class CScene {
public:
    CPlayerObject* GetPlayer(int) const;

    unsigned char unknown000[328];
};

extern CScene g_scene;
extern BSObjectView* g_pBSObject;
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

void BIFunc_PlayerMotionShake(int** stack, void*) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    float strength = *(float*)(*stack - (count - 1));
    float time = *(float*)(*stack - (count - 2));
    CPlayerObject* player = g_scene.GetPlayer(0);
    if (player)
        player->DoMotionShake(strength, time);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_PlayerCanCrouch(int** stack, void*) {
    bool canCrouch = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1)) != 0;
    CPlayerObject* player = g_scene.GetPlayer(0);
    if (player)
        player->m_canCrouch = canCrouch;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_PlayerStopCameraShake(int** stack, void*) {
    float time = *(float*)(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    CPlayerObject* player = g_scene.GetPlayer(0);
    if (g_bInMultiplayerMode && g_pBSObject && g_pBSObject->user && g_pBSObject->user->GetSceneNode() && g_pBSObject->user->GetSceneNode()->AsPlayerObject())
        player = g_pBSObject->user->GetSceneNode()->AsPlayerObject();
    if (player)
        player->StopCameraShake(time);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
