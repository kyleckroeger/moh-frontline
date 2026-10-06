// CAISoldierDoodad, the update, clean-up and initialisation functions in the
// middle of the file: the AI doodad that drives a soldier through a
// CAIFilterSoldierObject placed in the soldier's memory. The class names come
// from the mangled symbols. This is an inferred, non-virtual view: only the
// members these functions touch are declared, at their offsets (their names
// are not original), and the virtual table is not reproduced. The scene-node
// and fire functions before these and the destructor and constructor after
// them are not reconstructed here.
class CSoldierObject;
struct BSObject;

int BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);

class CAISoldierObject {
public:
    void Init();
};

class CAIFilterObject {
public:
    void LinkGameObject(void*);
};

class CAIFilterSoldierObject : public CAIFilterObject {
public:
    CAIFilterSoldierObject();
    virtual ~CAIFilterSoldierObject();
    void* operator new(unsigned long, void*);
    void Update(float);
    void PreUpdate();
    void SetOneTimeVars();
    void UpdateManualUpdateVars();

    unsigned char unknown04[4];
    CAISoldierObject* m_soldierObject;
    BSObject* m_script;
    unsigned char unknown10[4];
};

class CAIDoodad {
public:
    void Init();

    unsigned char unknown00[4];
};

class CAISoldierDoodad : public CAIDoodad {
public:
    void Update(float);
    void PreUpdate();
    void CleanUp();
    void Init(CSoldierObject*);

    CAIFilterSoldierObject* m_filter;
    int unknown08;
    int unknown0c;
    unsigned char unknown10[64];
    CSoldierObject* m_soldier;
};

void CAISoldierDoodad::Update(float time) {
    CAIFilterSoldierObject* filter = m_filter;

    filter->Update(time);
    if ((unknown08 & 1) && !(unknown0c & 1))
        BSObjectTriggerEvent(filter->m_script, 124, 0, 0, false);
}

void CAISoldierDoodad::PreUpdate() {
    m_filter->PreUpdate();
}

void CAISoldierDoodad::CleanUp() {
    delete m_filter;
}

void CAISoldierDoodad::Init(CSoldierObject* soldier) {
    m_filter = new ((char*)soldier + 17584) CAIFilterSoldierObject;
    m_filter->LinkGameObject(this);
    m_soldier = soldier;
    CAIDoodad::Init();
    CAIFilterSoldierObject* filter = m_filter;
    filter->SetOneTimeVars();
    filter->UpdateManualUpdateVars();
    filter->m_soldierObject->Init();
}
