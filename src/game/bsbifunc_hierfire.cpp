// A fragment of bsbifunc.cpp (0x8002013c): the script built-in that fires a
// sub-object weapon of the calling node's hierarchy tank (when it is one),
// followed by the weak CStaticObject::AsHierTankObject (0). It reads its
// argument below the script stack top and pops the built-in's arguments. The
// file name is this project's; the original record is bsbifunc.cpp and the
// built-ins around these are not reconstructed. The functions, classes and
// globals are named by the mangled symbols; ISceneNode is declared with its
// virtual functions in the order of __vt__10ISceneNode (AsHierObject at +124),
// IMovingSceneNode's and CStaticObject's after them in the order of
// __vt__13CStaticObject (AsHierTankObject at +328; the unnamed entry at +300 as
// a placeholder); the tank-object and built-in record views and the argument
// helper are inferred.
class CVector3 {
public:
    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

enum EClsnId {};
class CCollision;
class CDrawContext;
class CMatrix;
class CBullet;
class CLight;
class CPlayerObject;
class CStaticObject;
class CTankObject;
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
    virtual CStaticObject* AsHierObject();
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


class IMovingSceneNode : public ISceneNode {
public:
    virtual ~IMovingSceneNode();
    virtual void Reset();
    virtual void Halt();
    virtual void ApplyForceTo(CVector3, float);
    virtual void SetTMLocalToWorld(const CMatrix&);
};

class BSObject;
class BPDLightVolume;
enum EBSEventEnum {};

class CStaticObject : public IMovingSceneNode {
public:
    virtual ~CStaticObject();
    virtual void PreTransform(const CMatrix&);
    virtual void Transform(const CMatrix&);
    virtual void Move(CVector3);
    virtual void Rotate(CVector3, float);
    virtual void Pitch(float);
    virtual void Roll(float);
    virtual void Yaw(float);
    virtual void SetPosition(CVector3);
    virtual void SetBasis(CVector3, CVector3, CVector3);
    virtual void Orthonormalize();
    virtual int GetWorldLinearVelocity() const;
    virtual void EnterLightVolume(BPDLightVolume*);
    virtual void ExitLightVolume(BPDLightVolume*);
    virtual int GetLightVolume();
    virtual void Init();
    virtual void SetScript(BSObject*);
    virtual void unknown12c();
    virtual void TranslateFromScript(CVector3&, CVector3&, EBSEventEnum);
    virtual void RotateFromScript(CVector3&, CVector3&, EBSEventEnum);
    virtual void ScaleFromScript(float, float, EBSEventEnum);
    virtual void StartMotionPlayback(int, int, EBSEventEnum);
    virtual void* AsThrownObject();
    virtual void* AsWeaponObject();
    virtual CTankObject* AsHierTankObject();
};

class CTankObject {
public:
    void FireSubObject(int);
    void MapWeaponSlot(int, int);

    unsigned char unknown000[1940];
    int m_lookObject;
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

inline int BSArgInt(int** stack, int index) {
    return *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - index));
}

void BIFunc_HierObjectFire(int** stack, void* object) {
    int weapon = BSArgInt(stack, 1);
    CTankObject* tank = ((ISceneNode*)object)->AsHierObject()->AsHierTankObject();
    if (tank)
        tank->FireSubObject(weapon);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

__declspec(weak) CTankObject* CStaticObject::AsHierTankObject() {
    return 0;
}
