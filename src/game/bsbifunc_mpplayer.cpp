// A fragment of bsbifunc.cpp (0x8001eae4): multiplayer built-ins that return
// the script object's player's floco velocity and animated-player body (through
// the script object's scene node), popping the built-in's arguments. The file
// name is this project's; the original record is bsbifunc.cpp; DoMMGPointCheck
// before these is 8 instructions off. The functions, classes and globals are
// named by the mangled symbols; ISceneNode is declared with its virtual
// functions in the order of __vt__10ISceneNode, BSGO_Basic in the order of
// __vt__10BSGO_Basic and CAIFilterObject in the order of
// __vt__15CAIFilterObject; the player and built-in record views are inferred.
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

// BSGO_Basic, the object at BSObject+12: 12 bytes of members, then its
// virtual table pointer; the virtual functions it needs, in the order of
// __vt__10BSGO_Basic.
class BSGO_Basic {
    unsigned char unknown00[12];

public:
    virtual void Destroy();
    virtual void* GetScriptData();
    virtual ISceneNode* GetSceneNode();
};

struct BSObject {
    unsigned char unknown00[12];
    BSGO_Basic* user;
};
enum aifilter_target_mode {};
struct aistatus_match;

class CAIFilterObject {
public:
    virtual ~CAIFilterObject();
    virtual int ChooseTarget(float);
    virtual void SetTargetMode(aifilter_target_mode);
    virtual void ResetTargetMatchList(aifilter_target_mode);
    virtual void SetupTargetMatchList(aifilter_target_mode, aistatus_match*, int);
    int ShouldILeaveMMG(BSObject*);
};

struct AIDoodadView {
    void* unknown00;
    CAIFilterObject* filter;
};
union BSValueView {
    int i;
    float f;
};

class CAnimatedPlayer {
public:
    void GetFlocoParams(float&, long&);
};

class CPlayerObject {
public:
    unsigned char unknown000[1992];
    CAnimatedPlayer* m_animatedPlayer;
};

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

int DoMMGPointCheck(BSObject* checker, int other, int, int, int*, int* result);

void BIFunc_MPGetPlayerVelocity(int** stack, void*) {
    long frame;
    float velocity;
    CPlayerObject* player = g_pBSObject->user->GetSceneNode()->AsPlayerObject();
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    player->m_animatedPlayer->GetFlocoParams(velocity, frame);
    **stack = *(int*)&velocity;
}

void BIFunc_MPGetPlayerBody(int** stack, void*) {
    BSValueView value;
    CPlayerObject* player = g_pBSObject->user->GetSceneNode()->AsPlayerObject();
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
    value.i = (int)player->m_animatedPlayer;
    *(BSValueView*)*stack = value;
}

