// CThrownObject's destructor (a fragment of thrown_obj.cpp): resets the
// vtable and destroys the CStaticObject base. The names come from the mangled symbols;
// ISceneNode's virtual functions follow __vt__13CThrownObject (slots through
// GetWorldBoundingVolume, +48 as a placeholder) and CThrownObject declares
// OnCollision first so this file does not emit its vtable. BSGO_Basic keeps
// its three words before its vtable pointer, as the constructors store them;
// its first virtual entry is a placeholder. The members are inferred from
// offsets (the mesh-constructor that sets the orientation reads constants
// pooled with the rest of the file and is not part of these fragments).
class CCollision;
class CStaticMesh;
struct LevelFileContentsStruct_;

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
    virtual void unknown30();
    virtual void* GetLocalBoundingVolume(EVolumeType) const;
    virtual void* GetWorldBoundingVolume(EVolumeType) const;
};

class CStaticObject : public ISceneNode {
public:
    struct MotionFrame;

    CStaticObject(CStaticMesh*, int, int, int, int, int, MotionFrame*);
    CStaticObject();
    virtual ~CStaticObject();
};

class BSGO_Basic {
    int m_field0;
    int m_field4;
    int m_field8;

public:
    BSGO_Basic() : m_field0(0), m_field4(0), m_field8(0) {}
    virtual void unknown08();
};

class BSGO_Dummy : public BSGO_Basic {
public:
    virtual void unknown08();
};

void FreeSphereVolume(void*);

class CThrownObject : public CStaticObject {
public:
    void OnCollision(const CCollision&);
    void CleanUp();
    virtual ~CThrownObject();
    CThrownObject(CStaticMesh*, int, int, int, int, int, CStaticObject::MotionFrame*, LevelFileContentsStruct_*);
    CThrownObject();

    unsigned char unknown004[264];
    void* m_sphere;
    void* m_collider;
    void* m_lastCollider;
    unsigned char unknown118[416];
    float m_orientationX;
    float m_orientationY;
    float m_orientationZ;
    float m_orientationW;
    LevelFileContentsStruct_* m_definition;
    int m_pendingSound;
    BSGO_Dummy m_scriptObject;
    unsigned char unknown2E0[20];
    unsigned char flag0 : 1;
    unsigned char flag1 : 1;
    unsigned char flag2 : 1;
    unsigned char flag3 : 1;
    unsigned char deleted : 1;
    unsigned char markedForDestruction : 1;
    unsigned char flag6 : 1;
    unsigned char flag7 : 1;
};

CThrownObject::~CThrownObject() {
}
