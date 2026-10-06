// A fragment of bsbifunc.cpp (0x80028f8c): built-ins that add an objective
// (and raise the highest objective number), return the script object's
// embedded hit-reaction info, followed by BSGO_Basic's weak default for it
// (none), return an event-system memory block of 32 bytes, and destroy a
// light (marking it for destruction). Each reads its arguments below the
// script stack top, pops the built-in's arguments and writes a result to the
// new top through an integer union. The file name is this project's; the
// original record is bsbifunc.cpp and the built-ins around these are not
// reconstructed. The functions, classes and globals are named by the mangled
// symbols; BSGO_Basic's virtual functions are declared in the order of
// __vt__10BSGO_Basic (its virtual table pointer follows 12 bytes of
// members), ISceneNode in the order of __vt__10ISceneNode, and the built-in
// record and value views are inferred.
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

union BSValueView {
    int i;
    float f;
};

class BSGO_Basic {
    unsigned char unknown00[12];

public:
    virtual void Destroy();
    virtual void* GetScriptData();
    virtual void* GetSceneNode();
    virtual void* GetProximityData();
    virtual void* GetEmbeddedHitReactionInfo();
};

struct BSObjectView {
    unsigned char unknown00[12];
    BSGO_Basic* user;
};

extern BSObjectView* g_pBSObject;
extern int g_iHighestObjectiveNumber;

void AddObjective(unsigned int, unsigned int);
void* BSEventGetEventMemoryBlock(unsigned int);

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

void BIFunc_AddObjective(int** stack, void*) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    int objective = *(*stack - (count - 1));
    AddObjective(objective, *(*stack - (count - 2)));
    if (objective > g_iHighestObjectiveNumber)
        g_iHighestObjectiveNumber = objective;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_GetEmbeddedHitReactionInfo(int** stack, void*) {
    void* info = 0;
    info = g_pBSObject->user->GetEmbeddedHitReactionInfo();
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = *(int*)&info;
}

__declspec(weak) void* BSGO_Basic::GetEmbeddedHitReactionInfo() {
    return 0;
}

void BIFunc_GetEventSystemMemBlock(int** stack, void*) {
    BSValueView value;
    value.i = (int)BSEventGetEventMemoryBlock(32);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    *(BSValueView*)*stack = value;
}

void BIFunc_DestroyLight(int** stack, void*) {
    ((ISceneNode*)*(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1)))->MarkForDestruction(0);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
