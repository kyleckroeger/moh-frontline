// A fragment of bsbifunc.cpp (0x800206bc): the weak BSGO_Basic::GetScriptData
// (0), then script built-ins that stop a corpse search (unregistering the given
// registration record from the corpse-search schedule, or the calling node's
// script object when there is none) and restore the default fog (the colour and
// distances of the level's fog resource, type 13, alpha 128). Each reads its
// argument below the script stack top and pops the built-in's arguments. The
// file name is this project's; the original record is bsbifunc.cpp and the
// built-ins around these are not reconstructed. The functions, classes and
// globals are named by the mangled symbols; ISceneNode is declared with its
// virtual functions in the order of __vt__10ISceneNode (GetScriptObject at +96)
// and BSGO_Basic in the order of __vt__10BSGO_Basic; the schedule (BSSchedule's
// size is not known), colour (built by an inline constructor), resource and
// fog-resource views and the built-in record view are inferred.
enum EClsnId {};
class CCollision;
class CDrawContext;
class CMatrix;
class CVector3;
class CBullet;
class CLight;
class CPlayerObject;
struct AIDoodadView;
class BSObject;

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
    virtual BSObject* GetScriptObject() const;
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


class BSGO_Basic {
    unsigned char unknown00[12];

public:
    virtual void Destroy();
    virtual int GetScriptData();
};

struct BSScheduleRegistrationRecord_struct;

class BSSchedule {
public:
    void Unregister(BSScheduleRegistrationRecord_struct*);
    void Unregister(BSObject*);

    unsigned char unknown00[40];
};

extern BSSchedule g_CorpseSearchSchedule;

class CColor {
public:
    CColor(unsigned char ar, unsigned char ag, unsigned char ab, unsigned char aa) : r(ar), g(ag), b(ab), a(aa) {}

    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

enum TLTResourceID {};
struct LevelFileContentsStruct_;

struct FogResourceView {
    unsigned char unknown00[148];
    int red;
    unsigned char unknown98[4];
    int green;
    unsigned char unknowna0[4];
    int blue;
    unsigned char unknowna8[4];
    float nearDistance;
    unsigned char unknownb0[4];
    float farDistance;
};

struct TLTResourceView {
    unsigned char unknown00[8];
    FogResourceView* data;
};

TLTResourceView* TLT_FindNextResourceByType(TLTResourceID, LevelFileContentsStruct_*);
void SetFog(CColor&, float, float);

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

__declspec(weak) int BSGO_Basic::GetScriptData() {
    return 0;
}

void BIFunc_StopCorpseSearch(int** stack, void* object) {
    BSScheduleRegistrationRecord_struct* record = *(BSScheduleRegistrationRecord_struct**)(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    if (record)
        g_CorpseSearchSchedule.Unregister(record);
    else
        g_CorpseSearchSchedule.Unregister(((ISceneNode*)object)->GetScriptObject());
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_SetDefaultFogParams(int** stack, void*) {
    TLTResourceView* resource = TLT_FindNextResourceByType((TLTResourceID)13, 0);
    if (resource) {
        CColor color(0, 0, 0, 128);
        FogResourceView* fog = resource->data;
        color.r = fog->red;
        color.g = fog->green;
        color.b = fog->blue;
        SetFog(color, fog->nearDistance, fog->farDistance);
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}
