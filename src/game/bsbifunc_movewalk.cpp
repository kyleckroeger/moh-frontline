// A fragment of bsbifunc.cpp (0x80032208): AI built-ins that continue (walk
// state 8) or stop (walk state 0) a move-point walk, and start a walk to the
// move point held in the script object's instance data. Each reaches the script
// object's AI object through the scene node's AI doodad and its filter, and
// pops the built-in's arguments. The file name is this project's; the original
// record is bsbifunc.cpp; AISnapToMovePoint before these is 8 instructions off
// (a register swap in its three-float copy). The functions, classes and globals
// are named by the mangled symbols; ISceneNode is declared with its virtual
// functions in the order of __vt__10ISceneNode (GetScriptObject at +96,
// GetAIDoodad at +204), and the doodad, filter, CAIObject, script object and
// instance data views are inferred (members at their offsets, names not
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
struct BSObjectView;
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

class CAIFilterRealPosition {
public:
    CAIFilterRealPosition(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

    float x;
    float y;
    float z;
};

struct MovePointInstanceView {
    unsigned char unknown00[172];
    float x;
    float y;
    float z;
};

// The object at BSObject+12: 12 bytes of members, then its virtual table
// pointer; the second virtual function returns the instance data.
class BSObjectUserView {
    unsigned char unknown00[12];

public:
    virtual void unknownVirtual0();
    virtual MovePointInstanceView* GetInstanceData();
};

struct BSObjectView {
    unsigned char unknown00[12];
    BSObjectUserView* user;
};

class CAIObject {
public:
    void StartWalkToArbitraryPoint(CAIFilterRealPosition&);

    unsigned char unknown000[56];
    CAIFilterRealPosition m_position;
    unsigned char unknown044[8];
    bool m_flag4c;
    unsigned char unknown04d[347];
    unsigned int m_flags;
    unsigned char unknown1ac[108];
    int m_walkState;
    unsigned char unknown21c[172];
    float m_storedX;
    float m_storedY;
    float m_storedZ;
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

void BIFunc_AISnapToMovePoint(int** stack, void* object);

void BIFunc_AIContinueMovePointWalk(int** stack, void* object) {
    GetAIObject(object)->m_walkState = 8;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_AIStopMovePointWalk(int** stack, void* object) {
    GetAIObject(object)->m_walkState = 0;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_AIStartMovePointWalk(int** stack, void* object) {
    CAIFilterView* filter = ((ISceneNode*)object)->GetAIDoodad()->filter;
    MovePointInstanceView* data = ((ISceneNode*)object)->GetScriptObject()->user->GetInstanceData();
    CAIFilterRealPosition point(data->x, data->y, data->z);
    filter->object->StartWalkToArbitraryPoint(point);
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

