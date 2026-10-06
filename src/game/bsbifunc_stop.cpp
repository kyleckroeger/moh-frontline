// A fragment of bsbifunc.cpp (0x8002ddfc): built-ins that stop the buddy
// player, distance-to-target, target and target-search checks: a given
// registration record is unregistered from its schedule, otherwise every
// registration of the object (the script's object for the buddy check, the
// scene node's script object for the others), and the built-in's arguments
// are popped. The file name is this project's; the original record is
// bsbifunc.cpp and the built-ins around these are not reconstructed. The
// functions, classes and globals are named by the mangled symbols; ISceneNode
// is declared with its virtual functions in the order of __vt__10ISceneNode
// (GetScriptObject at +96), and the built-in record view is inferred.
enum EClsnId {};
class CCollision;
class CDrawContext;
class CMatrix;
class CVector3;
class CBullet;
class CLight;
class CPlayerObject;
struct BSObject;
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
    virtual BSObject* GetScriptObject() const;
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

struct BSScheduleRegistrationRecord_struct;

class BSSchedule {
public:
    void Unregister(BSScheduleRegistrationRecord_struct*);
    void Unregister(BSObject*);

    unsigned char unknown00[16];
};

extern BSSchedule g_BuddyPlayerCheckSchedule;
extern BSSchedule g_DistanceToTargetSchedule;
extern BSSchedule g_TargetCheckSchedule;
extern BSSchedule g_TargetSearchSchedule;
extern BSObject* g_pBSObject;

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

void BIFunc_StopBuddyPlayerCheck(int** stack, void*) {
    BSScheduleRegistrationRecord_struct* record =
        (BSScheduleRegistrationRecord_struct*)*(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    if (record)
        g_BuddyPlayerCheckSchedule.Unregister(record);
    else
        g_BuddyPlayerCheckSchedule.Unregister(g_pBSObject);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_StopDistanceToTarget(int** stack, void* object) {
    BSScheduleRegistrationRecord_struct* record =
        (BSScheduleRegistrationRecord_struct*)*(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    if (record)
        g_DistanceToTargetSchedule.Unregister(record);
    else
        g_DistanceToTargetSchedule.Unregister(((ISceneNode*)object)->GetScriptObject());
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_StopTargetCheck(int** stack, void* object) {
    BSScheduleRegistrationRecord_struct* record =
        (BSScheduleRegistrationRecord_struct*)*(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    if (record)
        g_TargetCheckSchedule.Unregister(record);
    else
        g_TargetCheckSchedule.Unregister(((ISceneNode*)object)->GetScriptObject());
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_StopTargetSearch(int** stack, void* object) {
    BSScheduleRegistrationRecord_struct* record =
        (BSScheduleRegistrationRecord_struct*)*(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    if (record)
        g_TargetSearchSchedule.Unregister(record);
    else
        g_TargetSearchSchedule.Unregister(((ISceneNode*)object)->GetScriptObject());
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
