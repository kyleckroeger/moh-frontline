// A fragment of bsbifunc.cpp (0x8003394c): AI built-ins that report a good
// time to fire (always), enable target aim (no effect), return the squared
// distance to the target (in 3D, or from the stored target position when the
// target flag is set; and in the plane), continue a spline path walk (states
// 2, 4 and 6 first continue the A* traversal; walk state 1) and stop it (walk
// state 0). Each reaches the script object's AI object through the scene
// node's AI doodad and its filter, and pops the built-in's arguments; results
// go through an integer/float union. The file name is this project's; the
// original record is bsbifunc.cpp and the built-ins around these are not
// reconstructed. The functions, classes and globals are named by the mangled
// symbols; ISceneNode is declared with its virtual functions in the order of
// __vt__10ISceneNode (GetAIDoodad at +204), and the doodad, filter, traversal
// and CAIObject views are inferred (members at their offsets, names not
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

class CAIFilterObject;

union BSValueView {
    int i;
    float f;
};

class CAISplinePathTraversal {
public:
    void ContinueTraversalForwardAStar(CAIFilterObject*);

    int m_state;
};

class CAIObject {
public:
    float GetDistanceSquaredXYZReal();
    float GetDistanceSquaredXYZReal(CAIObject*);
    float GetDistanceSquaredXYReal(CAIObject*);

    unsigned char unknown000[8];
    CAIFilterObject* m_filter;
    unsigned char unknown00c[180];
    bool m_targetStored;
    unsigned char unknown0c1[3];
    CAIObject* m_target;
    unsigned char unknown0c8[336];
    int m_walkState;
    unsigned char unknown21c[20];
    CAISplinePathTraversal m_traversal;
};

struct CAIFilterView {
    unsigned char unknown00[8];
    CAIObject* object;
};

struct AIDoodadView {
    void* unknown00;
    CAIFilterView* filter;
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

inline CAIObject* GetAIObject(void* object) {
    return ((ISceneNode*)object)->GetAIDoodad()->filter->object;
}

void BIFunc_AIIsItAGoodTimeToFire(int** stack, void* object) {
    ((ISceneNode*)object)->GetAIDoodad();
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = 1;
}

void BIFunc_AIEnableTargetAim(int** stack, void*) {
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_AIGetDistanceToTargetXYZ(int** stack, void* object) {
    BSValueView value;
    CAIObject* ai = GetAIObject(object);
    if (ai->m_targetStored)
        value.f = ai->GetDistanceSquaredXYZReal();
    else
        value.f = ai->GetDistanceSquaredXYZReal(ai->m_target);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    *(BSValueView*)*stack = value;
}

void BIFunc_AIGetDistanceToTargetXY(int** stack, void* object) {
    BSValueView value;
    CAIObject* ai = GetAIObject(object);
    value.f = ai->GetDistanceSquaredXYReal(ai->m_target);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    *(BSValueView*)*stack = value;
}

void BIFunc_AIContinueSplinePathWalk(int** stack, void* object) {
    CAIObject* ai = GetAIObject(object);
    switch (ai->m_traversal.m_state) {
    case 1:
    case 3:
    case 5:
        ai->m_walkState = 1;
        break;
    case 2:
    case 4:
    case 6:
        ai->m_traversal.ContinueTraversalForwardAStar(ai->m_filter);
        ai->m_walkState = 1;
        break;
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_AIStopSplinePathWalk(int** stack, void* object) {
    GetAIObject(object)->m_walkState = 0;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
