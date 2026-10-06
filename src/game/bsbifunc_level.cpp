// A fragment of bsbifunc.cpp (0x80021b60): built-ins that end the level
// (recording whether it was won, destroying the buddy data and setting the
// game-state end bit), add a hint (a level of 0x7fffffff stands for 20; the
// text comes from the string table, or null past its end; levels and slots are
// 1-based), toggle the player's trigger checks and change the player's team
// (the AI object's team and its byte copy), followed by the weak
// CPlayerObject::GetAIDoodad (the doodad at +0x24). The file name is this
// project's; the original record is bsbifunc.cpp and the built-ins around these
// are not reconstructed. The functions, classes and globals are named by the
// mangled symbols; ISceneNode is declared with its virtual functions in the
// order of __vt__10ISceneNode, and the shell, string table, hint table, player,
// doodad and AI object views are inferred (members at their offsets, names not
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

struct CAIObjectTeamView {
    unsigned char unknown000[20];
    signed char teamByte;
    unsigned char unknown015[435];
    int team;
};

struct CAIFilterView {
    unsigned char unknown00[8];
    CAIObjectTeamView* object;
};

struct AIDoodadView {
    void* unknown00;
    CAIFilterView* filter;
};

class CPlayerObject {
public:
    AIDoodadView* GetAIDoodad();

    unsigned char unknown000[36];
    AIDoodadView m_doodad;
    unsigned char unknown02c[876];
    unsigned char m_triggerChecks : 1;
    unsigned char unknown398 : 7;
};

struct ShellEndView {
    unsigned char unknown00[15];
    bool levelWon;
};

struct StringTableEntryView {
    const char* text;
    int unknown04;
};

struct StringTableView {
    StringTableEntryView* entries;
    unsigned char unknown04[8];
    int count;
};

extern ShellEndView g_Shell;
extern unsigned int g_uiGameState;
extern StringTableView umTable;
extern const char* g_HintTextArray[][3];

void DestroyBuddyDataStructures();

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

void BIFunc_EndLevel(int** stack, void*) {
    g_Shell.levelWon = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1)) != 0;
    DestroyBuddyDataStructures();
    g_uiGameState |= 1;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_AddHint(int** stack, void*) {
    const char* text;
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    int level = *(*stack - (count - 1));
    int slot = *(*stack - (count - 2));
    int string = *(*stack - (count - 3));
    if (level == 0x7fffffff)
        level = 20;
    if (string < umTable.count)
        text = umTable.entries[string].text;
    else
        text = 0;
    g_HintTextArray[level - 1][slot - 1] = text;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_TogglePlayerVsTriggerChecks(int** stack, void* object) {
    int enable = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    CPlayerObject* player = ((ISceneNode*)object)->AsPlayerObject();
    player->m_triggerChecks = enable != 0;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_ChangePlayersTeam(int** stack, void* object) {
    int team = *(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    CAIFilterView* filter = ((ISceneNode*)((ISceneNode*)object)->AsPlayerObject())->GetAIDoodad()->filter;
    filter->object->team = team;
    filter->object->teamByte = team;
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

__declspec(weak) AIDoodadView* CPlayerObject::GetAIDoodad() {
    return &m_doodad;
}
