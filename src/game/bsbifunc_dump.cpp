// A fragment of bsbifunc.cpp (0x800269f0): script built-ins that create a
// dump object at the calling node's position and its script trigger's
// rotation (the dump type and flag read from the trigger for trigger types
// 2, 3 and 8; nothing for type 0 or -1) and set whether the current weapon
// of the node's soldier is drawn (a flag bit of the weapon), and make the
// calling player's current weapon stop shooting (a shot of type 1; the
// constants are entries of the file's .sdata2 pool). Each reads its argument
// below the script stack top and pops the built-in's arguments. The file
// name is this project's; the original record is bsbifunc.cpp and the
// built-ins around these are not reconstructed. The functions, classes and
// globals are named by the mangled symbols; ISceneNode is declared with its
// virtual functions in the order of __vt__10ISceneNode (GetPosition at +64,
// GetScriptObject at +96, AsSoldierObject at +148); the vector view (four
// floats, 8-byte aligned), the quaternion view (built by an inline
// constructor), the trigger, script-object, soldier, weapon and built-in
// record views are inferred (members at their offsets, names not original).
class CVector3 {
public:
    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CQuaternion {
public:
    CQuaternion(float ax, float ay, float az, float aw) : x(ax), y(ay), z(az), w(aw) {}

    float x;
    float y;
    float z;
    float w;
};

enum EClsnId {};
class CCollision;
class CDrawContext;
class CMatrix;
class CBullet;
class CLight;
class CPlayerObject;
struct AIDoodadView;
struct BSObjectView;

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
    virtual BSObjectView* GetScriptObject() const;
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
    virtual CPlayerObject* AsSoldierObject();
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


struct TriggerCoreView {
    unsigned char unknown00[12];
    int type;
    unsigned char unknown10[12];
    float rx;
    float ry;
    float rz;
    float rw;
    unsigned char unknown2c[69];
    unsigned char dumpFlag;
    unsigned char unknown72[2];
    int dumpType;
    unsigned char unknown78[24];
    int dumpType3;
};

struct TriggerObject_struct {
    unsigned char unknown00[4];
    TriggerCoreView* core;
};

struct BSObjectView {
    unsigned char unknown00[8];
    TriggerObject_struct* trigger;
};

enum EWeaponShootType {};

class CWeapon {
public:
    void Shoot(EWeaponShootType, float, float);

    unsigned char unknown000[480];
    unsigned char m_drawn : 1;
    unsigned char unknown1e0 : 7;
};

class CPlayerObject {
public:
    CWeapon* GetCurrentWeapon() const;

    unsigned char unknown0000[16400];
    CWeapon* m_weapons[25];
    int m_currentWeapon;
};

void CreateDumpObject(const CVector3&, const CQuaternion&, int, int);

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

void BIFunc_CreateDumpObject(int** stack, void* object) {
    ISceneNode* node = (ISceneNode*)object;
    TriggerCoreView* core = node->GetScriptObject()->trigger->core;
    int flag;
    int type;
    switch (core->type) {
    case 3:
        flag = core->dumpFlag;
        type = core->dumpType3;
        break;
    case 8:
        flag = core->dumpFlag;
        type = core->dumpType;
        break;
    case 2:
        flag = core->dumpFlag;
        type = core->dumpType;
        break;
    default:
        flag = 0;
        type = 0;
        break;
    }
    if (type != 0 && type != 0xffffffff) {
        CVector3 position;
        CQuaternion rotation(core->rx, core->ry, core->rz, core->rw);
        node->GetPosition(position);
        CreateDumpObject(position, rotation, type, flag);
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_SetWeaponDraw(int** stack, void* object) {
    bool draw = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1)) != 0;
    CPlayerObject* soldier = ((ISceneNode*)object)->AsSoldierObject();
    CWeapon* weapon = soldier->m_weapons[soldier->m_currentWeapon];
    if (weapon)
        weapon->m_drawn = draw;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_DisablePlayerShoot(int** stack, void* object) {
    CPlayerObject* player = ((ISceneNode*)object)->AsPlayerObject();
    if (player->GetCurrentWeapon())
        player->GetCurrentWeapon()->Shoot((EWeaponShootType)1, 0.0f, -1.0f);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
