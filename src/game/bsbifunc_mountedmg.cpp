// A fragment of bsbifunc.cpp (0x800218d4): the script built-in that attaches
// the calling node's player to a mounted machine gun or releases it: the gun is
// the scene node (a static object in the box list or a hierarchy object in the
// tank list) whose script object has the given script object's trigger. It
// reads its arguments below the script stack top and pops the built-in's
// arguments. The file name is this project's; the original record is
// bsbifunc.cpp and the built-ins around it are not reconstructed. The
// functions, classes and globals are named by the mangled symbols; ISceneNode
// is declared with its virtual functions in the order of __vt__10ISceneNode
// (GetScriptObject at +96, AsStaticObject at +116, AsHierObject at +124,
// AsPlayerObject at +156) and BSGO_Basic in the order of __vt__10BSGO_Basic
// (the list link at +4); the node search is an inferred inline helper, and the
// script-object and built-in record views are inferred.
enum EClsnId {};
class CCollision;
class CDrawContext;
class CMatrix;
class CVector3;
class CBullet;
class CLight;
class CPlayerObject;
struct AIDoodadView;
struct BSObjectView;
class CStaticObject;

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
    virtual BSObjectView* GetScriptObject() const;
    virtual void TriggerScriptEvent(int, void*, bool);
    virtual void HandleBulletCollision(CBullet*, const CCollision&);
    virtual void* AsMovingNode();
    virtual const void* AsMovingNode() const;
    virtual CStaticObject* AsStaticObject();
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


class CPlayerObject {
public:
    void SetUsingMountedMachineGun(bool, CStaticObject*);
};

struct TriggerObject_struct;

struct BSObjectView {
    unsigned char unknown00[8];
    TriggerObject_struct* trigger;
};

class BSGO_Basic {
    unsigned char unknown00[4];

public:
    BSGO_Basic* next;
    unsigned char unknown08[4];
    virtual void Destroy();
    virtual int GetScriptData();
    virtual ISceneNode* GetSceneNode();
};

extern BSGO_Basic* g_pBoxList;
extern BSGO_Basic* g_pTankList;

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

inline ISceneNode* FindMMGNode(TriggerObject_struct* trigger) {
    BSGO_Basic* user;
    ISceneNode* node;
    BSObjectView* script;
    for (user = g_pBoxList; user; user = user->next) {
        node = user->GetSceneNode();
        if (node && node->AsStaticObject() && (script = node->GetScriptObject()) && script->trigger == trigger)
            return node;
    }
    for (user = g_pTankList; user; user = user->next) {
        node = user->GetSceneNode();
        if (node && node->AsHierObject() && (script = node->GetScriptObject()) && script->trigger == trigger)
            return node;
    }
    return 0;
}

void BIFunc_AttachPlayerToMountedMachineGun(int** stack, void* object) {
    TriggerObject_struct* trigger;
    BSObjectView* gun = *(BSObjectView**)(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    int attach = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 2));
    trigger = gun->trigger;
    ((ISceneNode*)object)->AsPlayerObject()->SetUsingMountedMachineGun(attach != 0, (CStaticObject*)FindMMGNode(trigger));
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
