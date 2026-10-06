// A fragment of bsbifunc.cpp (0x8002f298): hierarchy-object built-ins that
// destroy a sub-object (or, for id 0, schedule the script object's own
// destruction), swap two sub-objects and create a child sub-object with an
// attachment. Each reads its arguments below the script stack top before
// taking the scene node's hierarchy object, and pops the built-in's
// arguments. The file name is this project's; the original record is
// bsbifunc.cpp and the built-ins around these are not reconstructed. The
// functions, classes and globals are named by the mangled symbols; ISceneNode
// is declared with its virtual functions in the order of __vt__10ISceneNode
// (AsHierObject at +124), and the script-object and built-in record views are
// inferred.
enum EClsnId {};
class CCollision;
class CDrawContext;
class CMatrix;
class CVector3;
class CBullet;
class CLight;
class CPlayerObject;
class CHierObject;
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
    virtual CHierObject* AsHierObject();
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

enum DWI_ATTACHMENT_CRC_ENUM {};
class BSGO_Basic;

struct BSObjectView {
    unsigned char unknown00[12];
    BSGO_Basic* user;
};

extern BSObjectView* g_pBSObject;

void DelayDestroyObject(BSGO_Basic*);
void DestroySubHierObject(CHierObject*, int);
void SwapHierObject(CHierObject*, int, int);
void CreateSubHierObject(CHierObject*, int, int, DWI_ATTACHMENT_CRC_ENUM);

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

void BIFunc_DestroySubHierObject(int** stack, void* object) {
    int id = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    CHierObject* hier = ((ISceneNode*)object)->AsHierObject();
    if (id == 0)
        DelayDestroyObject(g_pBSObject->user);
    else
        DestroySubHierObject(hier, id);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_SwapHierObject(int** stack, void* object) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    int first = *(*stack - (count - 1));
    int second = *(*stack - (count - 2));
    CHierObject* hier = ((ISceneNode*)object)->AsHierObject();
    SwapHierObject(hier, first, second);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_CreateChildSubHierObject(int** stack, void* object) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    int parent = *(*stack - (count - 1));
    int child = *(*stack - (count - 2));
    int attachment = *(*stack - (count - 3));
    CHierObject* hier = ((ISceneNode*)object)->AsHierObject();
    CreateSubHierObject(hier, parent, child, (DWI_ATTACHMENT_CRC_ENUM)attachment);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
