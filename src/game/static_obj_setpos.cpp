// A fragment of static_obj.cpp (0x800af90c): CStaticObject's PlayAnimation
// (clamps the frame range and records speed, range and event, reversing for a
// negative speed over a descending range), the light-volume forwards to its
// manager (+608), SetBasis (sets the transform's rows and refreshes the
// rotation at +384) and SetPosition, forwarding to the object's transform
// (a CMatrix at +64). The animation fields and flag bits are inferred. The file
// name is this project's; the original record is static_obj.cpp and the functions
// around these are not reconstructed. CStaticObject and CMatrix's methods are named by
// the mangled symbols; CStaticObject is an inferred non-virtual view with only the
// transform declared, the bodies are inferred from the calls, and the by-value
// row getters are inferred inline helpers.
//
// CVector3 view: four floats, 8-byte aligned, overlaid with two doubles. The
// union is inferred from the code, not the original declaration: vectors are
// copied as two lfd/stfd pairs, which an implicit copy through the double pair
// reproduces.
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
    void RotateX(float);
    void RotateY(float);
    void RotateZ(float);
    void Rotate(CVector3, float);
    void Translate(CVector3);
    void SetRight(CVector3);
    void SetFront(CVector3);
    void SetUp(CVector3);
    void SetPos(CVector3);
    void Multiply(const CMatrix&, const CMatrix&);
    void Ident();

    CVector3 right;
    CVector3 forward;
    CVector3 up;
    CVector3 position;
};

struct BPDLightVolume;
enum EBSEventEnum {};

class CQuaternion {
public:
    void SetFromMatrix(const CMatrix&);

    float x;
    float y;
    float z;
    float w;
};

class CLightVolumeManager {
public:
    BPDLightVolume* GetVolume();
    void RemoveVolume(BPDLightVolume*);
    void AddVolume(BPDLightVolume*);
};

class CStaticObject {
public:
    void PlayAnimation(int, int, int, EBSEventEnum);
    BPDLightVolume* GetLightVolume();
    void ExitLightVolume(BPDLightVolume*);
    void EnterLightVolume(BPDLightVolume*);
    void SetBasis(CVector3, CVector3, CVector3);
    void SetPosition(CVector3);

    CVector3 UpRow() const { return m_tm.up; }
    CVector3 ForwardRow() const { return m_tm.forward; }
    CVector3 RightRow() const { return m_tm.right; }
    CVector3 PositionRow() const { return m_tm.position; }

    unsigned char unknown00[64];
    CMatrix m_tm;
    unsigned char unknown080[256];
    CQuaternion m_rotation;
    unsigned char unknown190[80];
    unsigned char m_flag80 : 1;
    unsigned char m_flag40 : 1;
    unsigned char m_flag20 : 1;
    unsigned char m_flag10 : 1;
    unsigned char m_flag08 : 1;
    unsigned char m_flag04 : 1;
    unsigned char m_reverse : 1;
    unsigned char m_flag01 : 1;
    unsigned char unknown1e1[27];
    EBSEventEnum m_event;
    int m_start;
    int m_end;
    int m_frameCount;
    unsigned char unknown20c[4];
    int m_speed;
    int m_frame;
    unsigned char unknown218[72];
    CLightVolumeManager m_lightVolumes;
};

void CStaticObject::PlayAnimation(int speed, int start, int end, EBSEventEnum event) {
    if (speed < 0 && start > end) {
        m_reverse = 1;
        speed = -speed;
    } else if (speed < 0) {
        m_reverse = 0;
        speed = 0;
    } else {
        m_reverse = 0;
    }
    if (start < 0)
        start = 0;
    if (start >= m_frameCount)
        start = m_frameCount - 1;
    if (end < 0)
        end = 0;
    if (end >= m_frameCount)
        end = m_frameCount;
    m_speed = speed;
    m_start = start;
    m_frame = start;
    m_end = end;
    m_event = event;
}

BPDLightVolume* CStaticObject::GetLightVolume() {
    return m_lightVolumes.GetVolume();
}

void CStaticObject::ExitLightVolume(BPDLightVolume* volume) {
    m_lightVolumes.RemoveVolume(volume);
}

void CStaticObject::EnterLightVolume(BPDLightVolume* volume) {
    m_lightVolumes.AddVolume(volume);
}

void CStaticObject::SetBasis(CVector3 right, CVector3 front, CVector3 up) {
    m_tm.SetRight(right);
    m_tm.SetFront(front);
    m_tm.SetUp(up);
    m_rotation.SetFromMatrix(m_tm);
}

void CStaticObject::SetPosition(CVector3 position) {
    m_tm.SetPos(position);
}
