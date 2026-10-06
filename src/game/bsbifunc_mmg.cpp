// A fragment of bsbifunc.cpp (0x8001f824): the weak
// CStaticObject::GetScriptObject (the script object at +280), then built-ins
// that choose a mounted-machine-gun point for the object's AI filter, stop
// the MMG check (a given registration record; otherwise every registration of
// the node's script object, removed from the buddy-player schedule as the
// original does), and register the MMG check for the script object, returning
// the registration. Each reads its argument below the script stack top and
// pops the built-in's arguments; results the original keeps in memory are
// written through their address. The file name is this project's; the
// original record is bsbifunc.cpp and the built-ins around these are not
// reconstructed. The functions, classes and globals are named by the mangled
// symbols; ISceneNode is declared with its virtual functions in the order of
// __vt__10ISceneNode, and the static-object, doodad and built-in record views
// are inferred.
enum EClsnId {};
class CCollision;
class CDrawContext;
class CMatrix;
class CVector3;
class CBullet;
class CLight;
struct BSObject;
class CPlayerObject;
struct AIDoodadView;
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
    BSScheduleRegistrationRecord_struct* Register(BSObject*, unsigned short, bool, int, int, int);

    unsigned char unknown00[16];
};

class CAIFilterObject {
public:
    int ChooseMMGPoint();
};

struct AIDoodadView {
    void* unknown00;
    CAIFilterObject* filter;
};

class CStaticObject {
public:
    BSObject* GetScriptObject() const;

    unsigned char unknown000[280];
    BSObject* m_script;
};

extern BSSchedule g_MMGPointCheckSchedule;
extern BSSchedule g_BuddyPlayerCheckSchedule;
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

__declspec(weak) BSObject* CStaticObject::GetScriptObject() const {
    return m_script;
}

void BIFunc_AIChooseMMGPoint(int** stack, void* object) {
    int point = 0;
    point = ((ISceneNode*)object)->GetAIDoodad()->filter->ChooseMMGPoint();
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = *(int*)&point;
}

void BIFunc_StopMMGCheck(int** stack, void* object) {
    BSScheduleRegistrationRecord_struct* record =
        (BSScheduleRegistrationRecord_struct*)*(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    if (record)
        g_MMGPointCheckSchedule.Unregister(record);
    else
        g_BuddyPlayerCheckSchedule.Unregister(((ISceneNode*)object)->GetScriptObject());
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_RegisterMMGCheck(int** stack, void*) {
    BSScheduleRegistrationRecord_struct* record = g_MMGPointCheckSchedule.Register(
        g_pBSObject, 0x102, true, *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1)), 0, 0);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = (int)record;
}
