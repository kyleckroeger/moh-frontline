// A fragment of bsbifunc.cpp (0x800341c0): the AI built-ins that reset the
// target match list and set the target mode of the script object's AI filter
// (through the scene node's AI doodad; the mode is the first argument), and
// DoSomethingInteresting, which sends the object towards the camera's
// position. Each pops the built-in's arguments. The file name is this
// project's; the original record is bsbifunc.cpp and the built-ins around these
// are not reconstructed. ISceneNode is declared with its virtual functions in
// the order of __vt__10ISceneNode (GetAIDoodad at +204); CAIFilterObject with
// the first virtual functions of __vt__15CAIFilterObject; the doodad view (its
// filter at +4) is inferred, as is the built-in record view.
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

enum aifilter_target_mode {};

class CAIFilterObject {
public:
    virtual ~CAIFilterObject();
    virtual void ChooseTarget(float);
    virtual void SetTargetMode(aifilter_target_mode);
    virtual void ResetTargetMatchList(aifilter_target_mode);
};

struct AIDoodadView {
    void* unknown00;
    CAIFilterObject* filter;
};

class CVector3 {
public:
    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CCamera {
public:
    void GetPosition(CVector3&) const;

    unsigned char unknown000[328];
};

class CAnimObject {
public:
    void SetMoveTarget(const CVector3&);
};

extern CCamera g_camera;

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

void BIFunc_AIResetTargetMatchList(int** stack, void* object) {
    ((ISceneNode*)object)->GetAIDoodad()->filter->ResetTargetMatchList((aifilter_target_mode)**stack);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_AISetTargetMode(int** stack, void* object) {
    ((ISceneNode*)object)->GetAIDoodad()->filter->SetTargetMode((aifilter_target_mode)**stack);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_DoSomethingInteresting(int** stack, void* object) {
    CVector3 position;
    g_camera.GetPosition(position);
    ((CAnimObject*)object)->SetMoveTarget(position);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
