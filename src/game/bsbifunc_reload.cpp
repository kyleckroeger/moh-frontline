// A fragment of bsbifunc.cpp (0x80030c0c): the player reload built-ins. Each
// takes the script object's player (ISceneNode::AsPlayerObject) and its
// current weapon, then finishes reloading, clears the reloading bit, reloads
// a single round or starts reloading, and pops the built-in's arguments from
// the script stack. The file name is this project's; the original record is
// bsbifunc.cpp and the functions around these are not reconstructed. The
// classes, functions and globals are named by the mangled symbols; ISceneNode
// is declared with its virtual functions in the order of __vt__10ISceneNode
// (as in the propdat.cpp fragments), CWeapon is an inferred view (the
// reloading bit in the flag byte at +704, cleared through an inferred inline
// helper), and the built-in record view is
// the one bsbifunc.cpp uses.
enum EClsnId {};
class CCollision;
class CDrawContext;
class CMatrix;
class CVector3;
class CBullet;
class CLight;
class CPlayerObject;

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
    virtual void* GetAIDoodad();
    virtual const void* GetAIDoodad() const;
    virtual void SetAttachedLight(CLight*, CVector3);
    virtual CLight* GetAttachedLight() const;
};

class CWeapon {
public:
    void DoneReloading();
    void ReloadSingle();
    void StartReloading();
    void ClearReloading() { reloading = 0; }

    unsigned char unknown000[704];
    unsigned char reloading : 1;
    unsigned char flags : 7;
};

class CPlayerObject {
public:
    CWeapon* GetCurrentWeapon() const;
};

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    unsigned char unknown0c[4];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

void BIFunc_PlayerFinishedReloading(int** stack, void* object) {
    ((ISceneNode*)object)->AsPlayerObject()->GetCurrentWeapon()->DoneReloading();
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_PlayerClearReloading(int** stack, void* object) {
    ((ISceneNode*)object)->AsPlayerObject()->GetCurrentWeapon()->ClearReloading();
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_PlayerReloadSingle(int** stack, void* object) {
    ((ISceneNode*)object)->AsPlayerObject()->GetCurrentWeapon()->ReloadSingle();
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_PlayerStartReloading(int** stack, void* object) {
    ((ISceneNode*)object)->AsPlayerObject()->GetCurrentWeapon()->StartReloading();
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
