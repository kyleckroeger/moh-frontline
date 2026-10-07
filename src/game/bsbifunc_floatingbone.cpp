// A fragment of bsbifunc.cpp (0x80020ab0): script built-ins that disable and
// enable a soldier's floating bone, detach the object attached to it, and
// attach a script object's scene node to it (the node is found by searching
// the box list for static objects and the tank list for hierarchical objects
// whose script object has the argument's trigger; the bone is "lt_hand" for
// mode 0 and "rt_hand" for mode 1). Each pops the built-in's arguments. The
// file name is this project's; the original record is bsbifunc.cpp and the
// built-ins around these are not reconstructed. The functions, globals and
// CSoldierObject's methods are named by the mangled symbols; CSoldierObject
// is a view whose virtual functions are AttachFloatingBone and
// DetachFloatingBone at +320 and +324 of __vt__14CSoldierObject, with
// placeholders for the 78 entries before them (their names are in that table
// and not needed here). The scene-node, script-object and box-list views are
// as in bsbifunc_mountedmg.cpp; the search helper, the built-in record view
// and the argument roles are inferred.
class CStaticObject;
class CVector3;
class CMatrix;

enum EClsnId {};
class CCollision;
class CDrawContext;
class CBullet;
class CLight;
class CPlayerObject;
struct AIDoodadView;
struct BSObjectView;

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

class CSoldierObject {
public:
    virtual void unknown008();
    virtual void unknown00c();
    virtual void unknown010();
    virtual void unknown014();
    virtual void unknown018();
    virtual void unknown01c();
    virtual void unknown020();
    virtual void unknown024();
    virtual void unknown028();
    virtual void unknown02c();
    virtual void unknown030();
    virtual void unknown034();
    virtual void unknown038();
    virtual void unknown03c();
    virtual void unknown040();
    virtual void unknown044();
    virtual void unknown048();
    virtual void unknown04c();
    virtual void unknown050();
    virtual void unknown054();
    virtual void unknown058();
    virtual void unknown05c();
    virtual void unknown060();
    virtual void unknown064();
    virtual void unknown068();
    virtual void unknown06c();
    virtual void unknown070();
    virtual void unknown074();
    virtual void unknown078();
    virtual void unknown07c();
    virtual void unknown080();
    virtual void unknown084();
    virtual void unknown088();
    virtual void unknown08c();
    virtual void unknown090();
    virtual void unknown094();
    virtual void unknown098();
    virtual void unknown09c();
    virtual void unknown0a0();
    virtual void unknown0a4();
    virtual void unknown0a8();
    virtual void unknown0ac();
    virtual void unknown0b0();
    virtual void unknown0b4();
    virtual void unknown0b8();
    virtual void unknown0bc();
    virtual void unknown0c0();
    virtual void unknown0c4();
    virtual void unknown0c8();
    virtual void unknown0cc();
    virtual void unknown0d0();
    virtual void unknown0d4();
    virtual void unknown0d8();
    virtual void unknown0dc();
    virtual void unknown0e0();
    virtual void unknown0e4();
    virtual void unknown0e8();
    virtual void unknown0ec();
    virtual void unknown0f0();
    virtual void unknown0f4();
    virtual void unknown0f8();
    virtual void unknown0fc();
    virtual void unknown100();
    virtual void unknown104();
    virtual void unknown108();
    virtual void unknown10c();
    virtual void unknown110();
    virtual void unknown114();
    virtual void unknown118();
    virtual void unknown11c();
    virtual void unknown120();
    virtual void unknown124();
    virtual void unknown128();
    virtual void unknown12c();
    virtual void unknown130();
    virtual void unknown134();
    virtual void unknown138();
    virtual void unknown13c();
    virtual void AttachFloatingBone();
    virtual void DetachFloatingBone();
    void DetachObjectFromFloatingBone();
    void AttachObjectToFloatingBone(CStaticObject*, char*, int);
};

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;


inline ISceneNode* FindNode(TriggerObject_struct* trigger) {
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

void BIFunc_DisableFloatingBone(int** stack, void* object) {
    ((CSoldierObject*)object)->DetachFloatingBone();
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_EnableFloatingBone(int** stack, void* object) {
    ((CSoldierObject*)object)->AttachFloatingBone();
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_DetachObjectFromFloatingBone(int** stack, void* object) {
    ((CSoldierObject*)object)->DetachObjectFromFloatingBone();
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_AttachObjectToFloatingBone(int** stack, void* object) {
    char* name = 0;
    TriggerObject_struct* trigger;
    BSObjectView* target = *(BSObjectView**)(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    int mode = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 2));
    int arg = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 3));
    CSoldierObject* soldier = (CSoldierObject*)object;
    trigger = target->trigger;
    ISceneNode* node = FindNode(trigger);
    switch (mode) {
    case 0:
        name = "lt_hand";
        break;
    case 1:
        name = "rt_hand";
        break;
    }
    soldier->AttachObjectToFloatingBone((CStaticObject*)node, name, arg);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
