// A fragment of anim_obj.cpp (0x80093804): CAnimObject::Orthonormalize (of its
// transform), SetYaw and GetYaw (the yaw kept as a 24-bit fraction of a turn:
// 2^24 / 2pi = 2670176.75 per radian) and Yaw (rotates the transform about Z
// and adds the angle to the stored yaw). The file name is this project's; the
// original record is anim_obj.cpp and the functions around these are not
// reconstructed. The class is the inferred non-virtual view used by
// anim_obj.cpp (members at their offsets); the file's globals are extern.
// CAnimObject, the first functions of the file: empty overrides, light-volume
// and scene bookkeeping, transform accessors and yaw. CAnimObject and the
// referenced classes are named by the mangled symbols. This is an inferred,
// non-virtual view: only the members these functions touch are declared, at
// their offsets, and the class's virtual table is not reproduced here.
class CVector3 {
public:
    float x;
    float y;
    float z;
    float w;
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
};

extern CScene g_scene;

class CAnimObject {
public:
    void UpdateLocalBoundingVolume(ISceneNode::EVolumeType, const CVector3&, const CVector3&);
    void AsAnimObject() const;
    void AsAnimObject();
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

void CAnimObject::UpdateLocalBoundingVolume(ISceneNode::EVolumeType, const CVector3&, const CVector3&);

void CAnimObject::AsAnimObject() const;

void CAnimObject::AsAnimObject();

int CAnimObject::AttachObject(CStaticObject*, char*, int);

int CAnimObject::DetachObject(char*);

int CAnimObject::GetAttachPointMatrix(char*, CMatrix*);

void CAnimObject::AttachFloatingBone();

void CAnimObject::DetachFloatingBone();

void CAnimObject::PreTransform(const CMatrix&);

void CAnimObject::Transform(const CMatrix&);

void CAnimObject::Rotate(CVector3, float);

void CAnimObject::Pitch(float);

void CAnimObject::Roll(float);

void CAnimObject::SetBasis(CVector3, CVector3, CVector3);

void CAnimObject::Destroy();

void CAnimObject::MarkForDestruction(int flag);

void CAnimObject::SetAttached(bool attached);

void CAnimObject::AttachToLocation(const CMatrix& location);

void CAnimObject::SetTMLocalToWorld(const CMatrix& matrix);

void* CAnimObject::GetLightVolume();

void CAnimObject::ExitLightVolume(BPDLightVolume* volume);

void CAnimObject::EnterLightVolume(BPDLightVolume* volume);

void CAnimObject::GetUpward(CVector3& v) const;

void CAnimObject::GetForward(CVector3& v) const;

void CAnimObject::GetRightward(CVector3& v) const;

void CAnimObject::GetPosition(CVector3& v) const;

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


