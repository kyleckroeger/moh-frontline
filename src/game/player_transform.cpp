// A fragment of player.cpp (0x800a0350): CPlayerObject's Transform and
// PreTransform, forwarding to the object's transform (a CMatrix at +128),
// then Halt (zeroes the velocity, the vector after it and the value at
// +676), Reset (identity transforms, zeroed vectors, angles and values, a
// few defaults, every weapon slot emptied and, in multiplayer, the soldier's
// weapons removed on each step, as compiled), GetCollisionId, IsDrawEnabled
// (always) and IsVisible (submits the draw data at +256 with the transform
// through the virtual at +16 of the object at +260, sets the context's value
// at +256 to 63). The file name is this project's; the original record is
// player.cpp. CPlayerObject, CMatrix's methods and the other functions are
// named by the mangled symbols; CPlayerObject, CDrawContext and the draw
// object are inferred views (members at their offsets, names not original),
// as is CVector3's chained zeroing helper.
class CVector3 {
public:
    void Zero() { x = y = z = 0.0f; }

    float x;
    float y;
    float z;
    float w;
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

enum EClsnId {};

class CDrawContext {
public:
    unsigned char unknown000[256];
    int m_value100;
};

// The object at +260 is called through the virtual at +16 of its table; the
// earlier slots are placeholders named by offset.
class CPlayerDrawView {
public:
    virtual void unknown008();
    virtual void unknown00c();
    virtual void unknown010(void*, const CMatrix&);
};

class CSoldierObject {
public:
    void RemoveWeapons();
};

class CPlayerWeaponObject;

extern bool g_bInMultiplayerMode;

class CPlayerObject {
public:
    void Transform(const CMatrix&);
    void PreTransform(const CMatrix&);
    void Halt();
    void Reset();
    EClsnId GetCollisionId() const;
    bool IsDrawEnabled() const;
    bool IsVisible(CDrawContext&) const;


    unsigned char unknown00[128];
    CMatrix m_tm;
    CMatrix m_cameraTM;
    void* m_drawData;
    CPlayerDrawView* m_drawView;
    unsigned char unknown108[336];
    CVector3 m_velocity;
    CVector3 m_vector268;
    unsigned char unknown278[16];
    CVector3 m_position;
    float m_value298;
    float m_yaw;
    float m_pitch;
    float m_value2a4;
    unsigned char unknown2a8[232];
    float m_value390;
    unsigned char unknown394a : 1;
    unsigned char m_flag394a : 1;
    unsigned char m_flag394b : 1;
    unsigned char unknown394b : 5;
    unsigned char unknown395[35];
    float m_value3b8;
    unsigned char unknown3bc[164];
    CPlayerWeaponObject* m_weapons[45];
    int m_soldierWeapons[45];
    unsigned char unknown5c8[12];
    int m_value5d4;
    int m_value5d8;
    int m_value5dc;
    unsigned char unknown5e0[236];
    EClsnId m_collisionId;
    unsigned char unknown6d0[112];
    CVector3 m_vector740;
    unsigned char unknown750[12];
    float m_value75c;
    unsigned char unknown760[104];
    CSoldierObject* m_soldier;
};

void CPlayerObject::Transform(const CMatrix& matrix) {
    m_tm.Multiply(m_tm, matrix);
}

void CPlayerObject::PreTransform(const CMatrix& matrix) {
    m_tm.Multiply(matrix, m_tm);
}

void CPlayerObject::Halt() {
    m_vector268.Zero();
    m_velocity.Zero();
    m_value2a4 = 0.0f;
}

void CPlayerObject::Reset() {
    m_tm.Ident();
    m_cameraTM.Ident();
    m_vector268.Zero();
    m_velocity.Zero();
    m_position.Zero();
    m_yaw = 0.0f;
    m_pitch = 0.0f;
    m_flag394a = 0;
    m_flag394b = 0;
    m_value3b8 = 0.0f;
    m_value390 = 0.0f;
    m_value75c = 1.0f;
    m_value298 = 1.1f;
    m_value2a4 = 0.0f;
    m_vector740.Zero();
    for (int i = 0; i < 45; i++) {
        m_weapons[i] = 0;
        m_soldierWeapons[i] = -1;
        if (g_bInMultiplayerMode && m_soldier)
            m_soldier->RemoveWeapons();
    }
    m_value5d4 = 0;
    m_value5d8 = 0;
    m_value5dc = 0;
}

EClsnId CPlayerObject::GetCollisionId() const {
    return m_collisionId;
}

bool CPlayerObject::IsDrawEnabled() const {
    return true;
}

bool CPlayerObject::IsVisible(CDrawContext& context) const {
    if (m_drawData)
        m_drawView->unknown010(m_drawData, m_tm);
    context.m_value100 = 63;
    return true;
}
