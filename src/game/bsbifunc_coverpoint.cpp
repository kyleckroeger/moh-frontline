// A fragment of bsbifunc.cpp (0x80033110): AI built-ins that report whether
// the AI object has arrived at a cover point (the cover point with the given
// index is its current one and its cover flag is set), continue a cover-point
// walk (walk state 9) and invalidate a cover point by id (when non-zero).
// Each reads its argument below the script stack top and pops the built-in's
// arguments. The file name is this project's; the original record is
// bsbifunc.cpp and the built-ins around these are not reconstructed. The
// functions, classes and globals are named by the mangled symbols; ISceneNode
// is declared with its virtual functions in the order of __vt__10ISceneNode
// (GetAIDoodad at +204), and the doodad, filter, CAIObject and built-in
// record views are inferred.
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

class CCoverPoint;

class CAIFilterGlobal {
public:
    CCoverPoint* GetCoverPointByIndex(int) const;
    void InvalidateCoverPointByID(int);

    unsigned char unknown00[24];
};

class CAIObject {
public:
    unsigned char unknown000[536];
    int m_walkState;
    unsigned char unknown21c[116];
    CCoverPoint* m_coverPoint;
    unsigned char unknown294[44];
    bool m_coverFlag;
};

struct CAIFilterView {
    unsigned char unknown00[8];
    CAIObject* object;
};

struct AIDoodadView {
    void* unknown00;
    CAIFilterView* filter;
};

extern CAIFilterGlobal g_aigAIFilterGlobalObject;

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

void BIFunc_AIArrivedAtCoverpoint(int** stack, void* object) {
    CAIFilterView* filter;
    int index = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    filter = ((ISceneNode*)object)->GetAIDoodad()->filter;
    CCoverPoint* point = g_aigAIFilterGlobalObject.GetCoverPointByIndex(index);
    CAIObject* ai = filter->object;
    bool arrived = false;
    if (point == ai->m_coverPoint && ai->m_coverFlag)
        arrived = true;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    **stack = arrived;
}

void BIFunc_AIContinueCoverPointWalk(int** stack, void* object) {
    GetAIObject(object)->m_walkState = 9;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_AIInvalidateCoverPointByID(int** stack, void*) {
    int id = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    if (id != 0)
        g_aigAIFilterGlobalObject.InvalidateCoverPointByID(id);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
