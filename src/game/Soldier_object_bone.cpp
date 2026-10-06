// A fragment of Soldier_object.cpp (0x800a9d04): CSoldierObject's floating
// bone controls (detach and attach set the attached flag when a floating
// object exists; detaching the object clears the flag and the object). The
// file name is this project's; the original record is Soldier_object.cpp and
// the functions around these are not reconstructed. CSoldierObject is named
// by the mangled symbols; its members are inferred.
// CSoldierObject, the functions at the start of the file: the AI doodad
// accessor and identity casts, script destruction, head tracking, fading,
// the bullet-emitter attach point and the floating bone. The class names come
// from the mangled symbols. This is an inferred, non-virtual view: only the
// members these functions touch are declared, at their offsets (their names
// are not original), and the virtual table is not reproduced. The accessor
// and casts are inline in the original (weak symbols), so they are defined
// __declspec(weak).
extern "C" int stricmp(const char*, const char*);

class CStaticObject;
class CAIDoodad;

class CMatrix {
public:
    CMatrix& operator=(const CMatrix&);

    float m[16];
};

// The object at BSObject+12; only its first virtual slot is called here, and
// its name is unknown (as in bsmachin.cpp).
class BSObjectUserView {
    unsigned char unknown00[12];

public:
    virtual void unknownVirtual0();
};

struct BSObject {
    unsigned char unknown00[12];
    BSObjectUserView* user;
};

class CWeapon {
public:
    unsigned char unknown000[716];
    void* m_emitter;
};

struct SoldierEmitterNameView {
    char name[24];
};

struct SoldierEmitterTableView {
    unsigned char unknown00[16];
    unsigned int count;
    SoldierEmitterNameView* names;
};

struct SoldierAttachPointView {
    CMatrix matrix;
    unsigned int id;
    unsigned char unknown44[12];
};

struct SoldierModelView {
    unsigned char unknown00[40];
    SoldierAttachPointView* attachPoints;
    int attachPointCount;
};

struct SoldierMeshView {
    unsigned char unknown00[4];
    SoldierModelView* model;
};

struct SoldierStaticObjectView {
    unsigned char unknown00[48];
    SoldierMeshView* mesh;
};

class CSoldierObject {
public:
    const CAIDoodad* GetAIDoodad() const;
    const CSoldierObject* AsSoldierObject() const;
    CSoldierObject* AsSoldierObject();
    void Destroy();
    void SetHeadTrans(bool);
    void SetHeadTrackPlayer(bool, int);
    void FadeOut(float);
    void SetBulletEmitter(CStaticObject*, char*);
    void DetachFloatingBone();
    void AttachFloatingBone();
    void DetachObjectFromFloatingBone();

    unsigned char unknown0000[3300];
    SoldierEmitterTableView* m_emitterTable;
    unsigned char unknown0ce8[5800];
    BSObject* m_script;
    unsigned char unknown2394[52];
    float m_headTurn0;
    float m_headTurn1;
    float m_headTurn2;
    unsigned char unknown23d4[3340];
    unsigned char m_aiDoodad[4];
    unsigned char unknown30e4[3804];
    bool m_floatingBoneAttached;
    unsigned char unknown3fc1[3];
    CStaticObject* m_floatingObject;
    unsigned char unknown3fc8[72];
    CWeapon* m_weapons[25];
    int m_currentWeapon;
    unsigned char unknown4078[520];
    bool m_emitterValid;
    unsigned char unknown4281[3];
    int m_emitterIndex;
    unsigned char unknown4288[8];
    CMatrix m_emitterMatrix;
    unsigned char unknown42d0[224];
    bool m_fading;
    unsigned char unknown43b1[3];
    float m_fadeTime;
    float m_fadeDuration;
    unsigned char unknown43bc[4];
    bool m_fadeFlag;
    bool m_headTrackPlayer;
    unsigned char unknown43c2[2];
    int m_headTrackTarget;
    int m_headTrackChanged;
    unsigned char unknown43cc[204];
    signed char m_headTransState;
};

__declspec(weak) const CAIDoodad* CSoldierObject::GetAIDoodad() const;

__declspec(weak) const CSoldierObject* CSoldierObject::AsSoldierObject() const;

__declspec(weak) CSoldierObject* CSoldierObject::AsSoldierObject();

void CSoldierObject::Destroy();

void CSoldierObject::SetHeadTrans(bool trans);

void CSoldierObject::SetHeadTrackPlayer(bool track, int target);

void CSoldierObject::FadeOut(float duration);

void CSoldierObject::SetBulletEmitter(CStaticObject* object, char* name);

void CSoldierObject::DetachFloatingBone() {
    if (m_floatingObject)
        m_floatingBoneAttached = false;
}

void CSoldierObject::AttachFloatingBone() {
    if (m_floatingObject)
        m_floatingBoneAttached = true;
}

void CSoldierObject::DetachObjectFromFloatingBone() {
    m_floatingBoneAttached = false;
    m_floatingObject = 0;
}


