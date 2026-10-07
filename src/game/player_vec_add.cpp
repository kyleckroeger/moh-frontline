// A fragment of player.cpp (0x800a4cec): the weak CVector3::Scale (a vector
// times a scalar) and Add. std::sqrtf after these uses the file's .sdata2
// pool and is not part of this unit, nor is the rest of the file.
// The CVector3 view (16 bytes, 8-byte aligned; its components and a double
// pair overlaid in a union, which gives the doubleword copies) and the CLine3
// view (as in capsule_create.cpp, with the inferred start-and-end setter named
// SetSE) are inferred. CVector3 and CLine3 and their functions are named by
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
};

__declspec(weak) void CVector3::Scale(const CVector3& a, float s) {
    d.v[0] = s * a.d.v[0];
    d.v[1] = s * a.d.v[1];
    d.v[2] = s * a.d.v[2];
}

__declspec(weak) void CVector3::Add(const CVector3& a) {
    d.v[0] += a.d.v[0];
    d.v[1] += a.d.v[1];
    d.v[2] += a.d.v[2];
}
