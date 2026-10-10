// The first function of line.cpp (0x800922f4): CLine3::IsDegenerate, whether
// the squared length of the direction (computed once and cached with its
// flag) is below 1e-12 (an entry of the file's .sdata2 pool). CLine3 is named
// by the mangled symbols; the layout (start, end and direction vectors, then
// the cached values and flags) is the view used in cdbobject_line.cpp, and the
// caching inline is inferred. The segment-distance and projection functions
// after it are not part of this unit.
union CVector3Data {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3Data d;
} __attribute__((aligned(8)));

class CLine3 {
public:
    bool IsDegenerate() const;
    float GetLengthSquared() const {
        if (!m_flag39) {
            m_value34 = m_dir.d.v[0] * m_dir.d.v[0] + m_dir.d.v[1] * m_dir.d.v[1] + m_dir.d.v[2] * m_dir.d.v[2];
            m_flag39 = 1;
        }
        return m_value34;
    }

    CVector3 m_start;
    CVector3 m_end;
    CVector3 m_dir;
    mutable float m_value30;
    mutable float m_value34;
    mutable unsigned char m_flag38;
    mutable unsigned char m_flag39;
} __attribute__((aligned(16)));

bool CLine3::IsDegenerate() const {
    return GetLengthSquared() < 1e-12f;
}
