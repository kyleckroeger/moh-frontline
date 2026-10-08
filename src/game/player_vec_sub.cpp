// A fragment of player.cpp (0x800a4dac): the weak CVector3::Sub, assignment
// (returning the vector by value), the CLine3 constructor from a start and an
// end point (direction end - start, flags cleared), the CVector3 constructor
// from three floats, and CPlayerObject::StartPlayerCrouched: it sets the
// crouched flag (clearing the next one), sets the value at +664 (0.7 in
// multiplayer, else 0; the constants are entries of the file's .sdata2 pool),
// rebuilds the local capsule's line from the origin to (0, 0, that value) and
// transforms it by the matrix at +128 into the world capsule. The rest of the
// file is not part of this unit.
// The CVector3 view (16 bytes, 8-byte aligned; its components and a double
// pair overlaid in a union, which gives the doubleword copies) and the CLine3
// view (as in capsule_create.cpp, with the inferred start-and-end setter named
// SetSE; 16-byte aligned, as the aligned stack frame of StartPlayerCrouched
// requires) and the CPlayerObject and capsule views are inferred. CVector3 and CLine3 and their functions are named by
// the mangled symbols; the functions are header inlines emitted as weak
// copies in this file, so they are defined __declspec(weak).
union CVector3Data {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(float, float, float);
    void Scale(const CVector3&, float);
    void Add(const CVector3&);
    void Sub(const CVector3&, const CVector3&);
    CVector3 operator=(const CVector3&);

    CVector3Data d;
} __attribute__((aligned(8)));

class CLine3 {
public:
    CLine3(CVector3, CVector3);
    CLine3& operator=(const CLine3& other) {
        if (&other != this) {
            m_start = other.m_start;
            m_end = other.m_end;
            m_dir = other.m_dir;
            m_value30 = other.m_value30;
            m_value34 = other.m_value34;
            m_flag38 = other.m_flag38;
            m_flag39 = other.m_flag39;
        }
        return *this;
    }

    void SetSE(CVector3 start, CVector3 end) {
        m_start.d = start.d;
        m_end.d = end.d;
        m_dir.d.v[0] = m_end.d.v[0] - m_start.d.v[0];
        m_dir.d.v[1] = m_end.d.v[1] - m_start.d.v[1];
        m_dir.d.v[2] = m_end.d.v[2] - m_start.d.v[2];
        m_flag38 = 0;
        m_flag39 = 0;
    }

    CVector3 m_start;
    CVector3 m_end;
    CVector3 m_dir;
    float m_value30;
    float m_value34;
    unsigned char m_flag38;
    unsigned char m_flag39;
} __attribute__((aligned(16)));

__declspec(weak) void CVector3::Sub(const CVector3& a, const CVector3& b) {
    d.v[0] = a.d.v[0] - b.d.v[0];
    d.v[1] = a.d.v[1] - b.d.v[1];
    d.v[2] = a.d.v[2] - b.d.v[2];
}

__declspec(weak) CVector3 CVector3::operator=(const CVector3& other) {
    d = other.d;
    return *this;
}

__declspec(weak) CLine3::CLine3(CVector3 start, CVector3 end) {
    SetSE(start, end);
}

__declspec(weak) CVector3::CVector3(float x, float y, float z) {
    d.v[0] = x;
    d.v[1] = y;
    d.v[2] = z;
}

class CMatrix;
class IVolume {
public:
    virtual ~IVolume();
    virtual IVolume* Create() const;
    virtual void TransformedCopy(const IVolume&, const CMatrix&);
};

class CVolCapsule : public IVolume {
public:
    virtual ~CVolCapsule();

    unsigned char unknown04[8];
    float m_value;
    CLine3 m_line;
};

extern bool g_bInMultiplayerMode;

class CPlayerObject {
public:
    void StartPlayerCrouched();

    unsigned char unknown000[128];
    unsigned char m_tm[64]; /* +128: the local-to-world matrix */
    unsigned char unknown0c0[240];
    CVolCapsule m_localCapsule;
    CVolCapsule m_worldCapsule;
    unsigned char unknown250[72];
    float m_crouchHeight;
    unsigned char unknown29c[249];
    unsigned char m_crouched : 1;
    unsigned char m_flag395b : 1;
    unsigned char unknown395c : 6;
};

void CPlayerObject::StartPlayerCrouched() {
    m_crouched = 1;
    m_flag395b = 0;
    m_crouchHeight = g_bInMultiplayerMode ? 0.7f : 0.0f;
    m_localCapsule.m_line = CLine3(CVector3(0.0f, 0.0f, 0.0f), CVector3(0.0f, 0.0f, m_crouchHeight));
    m_worldCapsule.TransformedCopy(m_localCapsule, *(CMatrix*)m_tm);
}
