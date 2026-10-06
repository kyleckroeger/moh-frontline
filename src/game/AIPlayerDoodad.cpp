// CAIPlayerDoodad, the functions at the start of the file: the AI doodad that
// drives a player object through a CAIFilterPlayerObject.
// The class names come from the mangled symbols. This is an inferred,
// non-virtual view of CAIPlayerDoodad: only the members these functions touch
// are declared, at their offsets, and its virtual table (with the inline
// CAIDoodad fire methods) is not reproduced, so the destructor and
// constructor that store it are not reconstructed here. GetSceneNode is
// inline in the original (a weak symbol), so it is defined __declspec(weak).
class ISceneNode;
class CPlayerObject;

class CAIPlayerObject {
public:
    void Init();
};

class CAIFilterObject {
public:
    void LinkGameObject(void*);
};

class CAIFilterPlayerObject : public CAIFilterObject {
public:
    CAIFilterPlayerObject();
    virtual ~CAIFilterPlayerObject();
    void Update(float);
    void PreUpdate();
    void SetOneTimeVars();
    void UpdateManualUpdateVars();

    unsigned char unknown04[4];
    CAIPlayerObject* m_playerObject;
    unsigned char unknown0c[12];
};

class CAIDoodad {
public:
    void Init();

    unsigned char unknown00[4];
};

class CAIPlayerDoodad : public CAIDoodad {
public:
    ISceneNode* GetSceneNode() const;
    void Update(float);
    void PreUpdate();
    void CleanUp();
    void Init(CPlayerObject*);

    CAIFilterPlayerObject* m_filter;
    unsigned char unknown08[72];
    CPlayerObject* m_player;
};

__declspec(weak) ISceneNode* CAIPlayerDoodad::GetSceneNode() const {
    return (ISceneNode*)m_player;
}

void CAIPlayerDoodad::Update(float time) {
    m_filter->Update(time);
}

void CAIPlayerDoodad::PreUpdate() {
    m_filter->PreUpdate();
}

void CAIPlayerDoodad::CleanUp() {
    delete m_filter;
}

void CAIPlayerDoodad::Init(CPlayerObject* player) {
    if (!m_filter)
        m_filter = new CAIFilterPlayerObject;
    m_filter->LinkGameObject(this);
    m_player = player;
    CAIDoodad::Init();
    CAIFilterPlayerObject* filter = m_filter;
    filter->SetOneTimeVars();
    filter->UpdateManualUpdateVars();
    filter->m_playerObject->Init();
}
