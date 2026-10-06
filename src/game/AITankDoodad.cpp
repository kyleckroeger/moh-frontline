// CAITankDoodad, the functions at the start of the file: the AI doodad that
// drives a tank through a CAIFilterTankObject placed in the tank's memory.
// The class names come from the mangled symbols. This is an inferred,
// non-virtual view of CAITankDoodad: only the members these functions touch
// are declared, at their offsets, and its virtual table (with the inline
// CAIDoodad fire methods) is not reproduced, so the destructor and
// constructor that store it are not reconstructed here. GetSceneNode is
// inline in the original (a weak symbol), so it is defined __declspec(weak).
class ISceneNode;
class CTankObject;

class CAITankObject {
public:
    void Init();
};

class CAIFilterObject {
public:
    void LinkGameObject(void*);
};

class CAIFilterTankObject : public CAIFilterObject {
public:
    CAIFilterTankObject();
    virtual ~CAIFilterTankObject();
    void* operator new(unsigned long, void*);
    void Update(float);
    void PreUpdate();
    void SetOneTimeVars();
    void UpdateManualUpdateVars();

    unsigned char unknown04[4];
    CAITankObject* m_tankObject;
    unsigned char unknown0c[8];
};

class CAIDoodad {
public:
    void Init();

    unsigned char unknown00[4];
};

class CAITankDoodad : public CAIDoodad {
public:
    ISceneNode* GetSceneNode() const;
    void Update(float);
    void PreUpdate();
    void CleanUp();
    void Init(CTankObject*);

    CAIFilterTankObject* m_filter;
    unsigned char unknown08[72];
    CTankObject* m_tank;
};

__declspec(weak) ISceneNode* CAITankDoodad::GetSceneNode() const {
    return (ISceneNode*)m_tank;
}

void CAITankDoodad::Update(float time) {
    m_filter->Update(time);
}

void CAITankDoodad::PreUpdate() {
    m_filter->PreUpdate();
}

void CAITankDoodad::CleanUp() {
    delete m_filter;
    m_filter = 0;
}

void CAITankDoodad::Init(CTankObject* tank) {
    m_filter = new ((char*)tank + 2096) CAIFilterTankObject;
    m_filter->LinkGameObject(this);
    m_tank = tank;
    CAIDoodad::Init();
    CAIFilterTankObject* filter = m_filter;
    filter->SetOneTimeVars();
    filter->UpdateManualUpdateVars();
    filter->m_tankObject->Init();
}
