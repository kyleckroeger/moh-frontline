// Script game-object creation, the dump container's destructor and the
// accessors of the script game-object classes that follow it. The class names
// come from the mangled symbols; the members are inferred from offsets and the
// accessors' result types are left incomplete. The accessors are inline in
// the original (weak symbols), so they are defined __declspec(weak). The
// creators before them and the static initialiser after them are not part of
// this unit.
class ISceneNode;
struct ProximityDataView;
struct ScriptDataView;
struct HitReactionInfoView;

class CDumpObjectContainer {
public:
    ~CDumpObjectContainer();

    char* m_objects;
};

CDumpObjectContainer::~CDumpObjectContainer() {
    delete[] m_objects;
    m_objects = 0;
}

class BSGO_Dummy {
public:
    ISceneNode* GetSceneNode();
    ProximityDataView* GetProximityData();

    unsigned char unknown00[16];
    ISceneNode* m_sceneNode;
    unsigned char m_proximity[4];
};

class BSGO_Destructable {
public:
    ScriptDataView* GetScriptData();
    ISceneNode* GetSceneNode();
    ProximityDataView* GetProximityData();

    unsigned char unknown00[16];
    unsigned char m_script[12];
    ISceneNode* m_sceneNode;
    unsigned char m_proximity[4];
};

class BSGO_Tank {
public:
    ScriptDataView* GetScriptData();
    ISceneNode* GetSceneNode();
    ProximityDataView* GetProximityData();

    unsigned char unknown00[16];
    unsigned char m_script[212];
    ISceneNode* m_sceneNode;
    unsigned char m_proximity[4];
};

class BSGO_Soldier {
public:
    ScriptDataView* GetScriptData();
    ISceneNode* GetSceneNode();
    ProximityDataView* GetProximityData();
    HitReactionInfoView* GetEmbeddedHitReactionInfo();

    unsigned char unknown00[16];
    unsigned char m_script[136];
    unsigned char m_hitReaction[76];
    ISceneNode* m_sceneNode;
    unsigned char m_proximity[4];
};

__declspec(weak) ISceneNode* BSGO_Dummy::GetSceneNode() {
    return m_sceneNode;
}

__declspec(weak) ProximityDataView* BSGO_Dummy::GetProximityData() {
    return (ProximityDataView*)m_proximity;
}

__declspec(weak) ScriptDataView* BSGO_Destructable::GetScriptData() {
    return (ScriptDataView*)m_script;
}

__declspec(weak) ISceneNode* BSGO_Destructable::GetSceneNode() {
    return m_sceneNode;
}

__declspec(weak) ProximityDataView* BSGO_Destructable::GetProximityData() {
    return (ProximityDataView*)m_proximity;
}

__declspec(weak) ScriptDataView* BSGO_Tank::GetScriptData() {
    return (ScriptDataView*)m_script;
}

__declspec(weak) ISceneNode* BSGO_Tank::GetSceneNode() {
    return m_sceneNode;
}

__declspec(weak) ProximityDataView* BSGO_Tank::GetProximityData() {
    return (ProximityDataView*)m_proximity;
}

__declspec(weak) ScriptDataView* BSGO_Soldier::GetScriptData() {
    return (ScriptDataView*)m_script;
}

__declspec(weak) ISceneNode* BSGO_Soldier::GetSceneNode() {
    return m_sceneNode;
}

__declspec(weak) ProximityDataView* BSGO_Soldier::GetProximityData() {
    return (ProximityDataView*)m_proximity;
}

__declspec(weak) HitReactionInfoView* BSGO_Soldier::GetEmbeddedHitReactionInfo() {
    return (HitReactionInfoView*)m_hitReaction;
}
