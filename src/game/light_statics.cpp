// A fragment of light.cpp (0x800785a4): CLight's default light volume, marking
// a light for destruction (removed from the scene first when it is in it, then
// the subject's marking), the empty Destroy, the two Create overloads and
// Register, which forward to the animated-light manager. CLight, CScene,
// ISceneNode, ISubject, CAnimLightManager, BPDLightVolume and
// MOH_animatedLight_Struct are named by the mangled symbols; CLight is a
// non-virtual view used as its scene node and subject (both at its start),
// the manager's registered members are inferred, and the result types are
// inferred. The rest of the file is not part of this unit.
struct BPDLightVolume;
struct MOH_animatedLight_Struct;
class ISceneNode;
class CLight;

class ISubject {
public:
    void MarkForDestruction(int);
};

class CScene {
public:
    bool IsNodeInScene(const ISceneNode*) const;
    void Remove(ISceneNode&);

    unsigned char data[16];
};

class CAnimLightManager {
public:
    CLight* Create(MOH_animatedLight_Struct*);
    CLight* Create(unsigned long);

    unsigned char unknown00[36];
    void* m_registered;
    int m_registeredCount;
};

extern CScene g_scene;
extern CAnimLightManager g_AnimLightManager;
extern BPDLightVolume g_DefaultLightVolume;

class CLight {
public:
    static BPDLightVolume* GetDefaultLightVolume();
    void MarkForDestruction(int);
    void Destroy();
    static CLight* Create(void*);
    static CLight* Create(unsigned long);
    static void Register(void*, int);
};

BPDLightVolume* CLight::GetDefaultLightVolume() {
    return &g_DefaultLightVolume;
}

void CLight::MarkForDestruction(int flag) {
    if (g_scene.IsNodeInScene((ISceneNode*)this))
        g_scene.Remove(*(ISceneNode*)this);
    ((ISubject*)this)->MarkForDestruction(flag);
}

void CLight::Destroy() {
}

CLight* CLight::Create(void* data) {
    return g_AnimLightManager.Create((MOH_animatedLight_Struct*)data);
}

CLight* CLight::Create(unsigned long id) {
    return g_AnimLightManager.Create(id);
}

void CLight::Register(void* lights, int count) {
    g_AnimLightManager.m_registered = lights;
    g_AnimLightManager.m_registeredCount = count;
}
