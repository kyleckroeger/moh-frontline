// CAnimObject, the first functions of the file: empty overrides, attachment
// flags, the transform setter and light-volume and scene bookkeeping. CAnimObject and the
// referenced classes are named by the mangled symbols. This is an inferred,
// non-virtual view: only the members these functions touch are declared, at
// their offsets, and the class's virtual table is not reproduced here. The
// empty overrides are inline in the class in the original (weak symbols,
// emitted for the vtable), so they are defined __declspec(weak) here.
// CVector3 view: four floats, 8-byte aligned, overlaid with two doubles. The
// union is inferred from the code, not the original declaration: whole-vector
// copies in this file (the row accessors) move each vector as two lfd/stfd
// pairs, which an implicit copy through the double pair reproduces.
class CVector3 {
public:
    union {
        struct {
            float x;
            float y;
            float z;
            float w;
        };
        double pair[2];
    };
} __attribute__((aligned(8)));

class CMatrix {
public:
    CMatrix& operator=(const CMatrix&);
    void Orthonormalize();
    void RotateZ(float);

    CVector3 right;
    CVector3 forward;
    CVector3 up;
    CVector3 position;
};

class CStaticObject;
class CDrawContext;
class BPDLightVolume;

class CLightVolumeManager {
public:
    void* GetVolume();
    void RemoveVolume(BPDLightVolume*);
    void AddVolume(BPDLightVolume*);
};

class ISceneNode {
public:
    enum EVolumeType { VolumeTypeUnknown = 0 };
};

class ISubject {
public:
    void MarkForDestruction(int);
};

class CScene {
public:
    void Remove(ISceneNode&);

    char data[328];
};

extern CScene g_scene;

class CAnimObject {
public:
    void UpdateLocalBoundingVolume(ISceneNode::EVolumeType, const CVector3&, const CVector3&);
    const CAnimObject* AsAnimObject() const;
    CAnimObject* AsAnimObject();
    int AttachObject(CStaticObject*, char*, int);
    int DetachObject(char*);
    int GetAttachPointMatrix(char*, CMatrix*);
    void AttachFloatingBone();
    void DetachFloatingBone();
    void PreTransform(const CMatrix&);
    void Transform(const CMatrix&);
    void Rotate(CVector3, float);
    void Pitch(float);
    void Roll(float);
    void SetBasis(CVector3, CVector3, CVector3);
    void Destroy();
    void MarkForDestruction(int);
    void SetAttached(bool);
    void AttachToLocation(const CMatrix&);
    void SetTMLocalToWorld(const CMatrix&);
    void* GetLightVolume();
    void ExitLightVolume(BPDLightVolume*);
    void EnterLightVolume(BPDLightVolume*);
    void GetUpward(CVector3&) const;
    void GetForward(CVector3&) const;
    void GetRightward(CVector3&) const;
    void GetPosition(CVector3&) const;
    void Orthonormalize();
    void SetYaw(float);
    float GetYaw() const;
    void Yaw(float);

    CVector3 GetUp() const { return m_tm.up; }
    CVector3 GetFwd() const { return m_tm.forward; }
    CVector3 GetRight() const { return m_tm.right; }
    CVector3 GetPos() const { return m_tm.position; }

    char field0000[9024];
    CMatrix m_tm;
    char field2380[48];
    int m_yaw;
    char field23B4[56];
    int m_field23EC;
    char field23F0[106];
    bool m_attached;
    bool m_attachedToLocation;
    char field245C[4];
    CMatrix m_attachLocation;
    char field24A0[24];
    CLightVolumeManager m_lightVolumes;
};

__declspec(weak) void CAnimObject::UpdateLocalBoundingVolume(ISceneNode::EVolumeType, const CVector3&, const CVector3&) {
}

__declspec(weak) const CAnimObject* CAnimObject::AsAnimObject() const {
    return this;
}

__declspec(weak) CAnimObject* CAnimObject::AsAnimObject() {
    return this;
}

__declspec(weak) int CAnimObject::AttachObject(CStaticObject*, char*, int) {
    return 0;
}

__declspec(weak) int CAnimObject::DetachObject(char*) {
    return 0;
}

__declspec(weak) int CAnimObject::GetAttachPointMatrix(char*, CMatrix*) {
    return 0;
}

__declspec(weak) void CAnimObject::AttachFloatingBone() {
}

__declspec(weak) void CAnimObject::DetachFloatingBone() {
}

__declspec(weak) void CAnimObject::PreTransform(const CMatrix&) {
}

__declspec(weak) void CAnimObject::Transform(const CMatrix&) {
}

__declspec(weak) void CAnimObject::Rotate(CVector3, float) {
}

__declspec(weak) void CAnimObject::Pitch(float) {
}

__declspec(weak) void CAnimObject::Roll(float) {
}

__declspec(weak) void CAnimObject::SetBasis(CVector3, CVector3, CVector3) {
}

void CAnimObject::Destroy() {
}

void CAnimObject::MarkForDestruction(int flag) {
    // The ISubject and ISceneNode bases are at the start of the object.
    ((ISubject*)this)->MarkForDestruction(flag);
    g_scene.Remove(*(ISceneNode*)this);
}

void CAnimObject::SetAttached(bool attached) {
    m_attached = attached;
    m_attachedToLocation = false;
}

void CAnimObject::AttachToLocation(const CMatrix& location) {
    m_attached = true;
    m_attachedToLocation = true;
    m_attachLocation = location;
}

void CAnimObject::SetTMLocalToWorld(const CMatrix& matrix) {
    m_tm = matrix;
}

void* CAnimObject::GetLightVolume() {
    return m_lightVolumes.GetVolume();
}

void CAnimObject::ExitLightVolume(BPDLightVolume* volume) {
    m_lightVolumes.RemoveVolume(volume);
}

void CAnimObject::EnterLightVolume(BPDLightVolume* volume) {
    m_lightVolumes.AddVolume(volume);
}

void CAnimObject::GetUpward(CVector3& v) const {
    v = GetUp();
}

void CAnimObject::GetForward(CVector3& v) const {
    v = GetFwd();
}

void CAnimObject::GetRightward(CVector3& v) const {
    v = GetRight();
}

void CAnimObject::GetPosition(CVector3& v) const {
    v = GetPos();
}

void CAnimObject::Orthonormalize() {
    m_tm.Orthonormalize();
}

void CAnimObject::SetYaw(float yaw) {
    m_yaw = (int)(2670176.75f * yaw) & 0xFFFFFF;
    m_field23EC = m_yaw;
}

float CAnimObject::GetYaw() const {
    return 6.2831855f * m_yaw / 16777216.0f;
}

void CAnimObject::Yaw(float angle) {
    m_tm.RotateZ(angle);
    m_yaw = (m_yaw + (int)(2670176.75f * angle)) & 0xFFFFFF;
}
