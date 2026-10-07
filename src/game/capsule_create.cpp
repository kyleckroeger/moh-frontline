// A fragment of capsule.cpp (0x800c72d4): CVolCapsule::TransformedCopy
// (copies the float and the capsule's line, then sets the line from its
// start and end transformed as points by the matrix) and CVolCapsule::Create
// (a new, default-constructed capsule of 80 bytes; the inline constructors set
// the IVolume and CVolCapsule virtual tables). IVolume, CVolCapsule, CLine3,
// CVector3 and CMatrix are named by the mangled symbols; IVolume's virtual
// functions are declared in the order of __vt__7IVolume and CVolCapsule
// declares its destructor first (defined elsewhere, so its virtual table is
// not emitted here). CLine3's layout follows the weak CLine3::SetSD (start,
// end, direction, two floats, two flags); its inline assignment and the
// start-and-end setter (named SetSE here; its original name is not known) are
// inferred, as are the capsule's members and the CVector3 view (16 bytes,
// copied as two doubles). The rest of the file is not part of this unit.
class CVector3 {
public:
    union {
        double pair[2];
        float v[4];
    };
} __attribute__((aligned(8)));

class CMatrix {
public:
    void TransformPoint(CVector3&, CVector3) const;
};
class CTriangle;
class CPlane;
class CLine3 {
public:
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
        m_start = start;
        m_end = end;
        m_dir.v[0] = m_end.v[0] - m_start.v[0];
        m_dir.v[1] = m_end.v[1] - m_start.v[1];
        m_dir.v[2] = m_end.v[2] - m_start.v[2];
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
class CVolBox;
class CVolSphere;
class CVolCapsule;
class CVolFiber;
class CCDBObject;
class CWorldVolume;
class CAnimatedVolume;
class CCollision;

class IVolume {
public:
    virtual ~IVolume();
    virtual IVolume* Create() const;
    virtual void TransformedCopy(const IVolume&, const CMatrix&);
    virtual bool TestCollision(const IVolume&, CCollision&, bool) const;
    virtual bool TestCollision(const CVolBox&, CCollision&, bool) const;
    virtual bool TestCollision(const CVolSphere&, CCollision&, bool) const;
    virtual bool TestCollision(const CVolCapsule&, CCollision&, bool) const;
    virtual bool TestCollision(const CCDBObject&, CCollision&, bool) const;
    virtual bool TestCollision(const CVolFiber&, CCollision&, bool) const;
    virtual bool TestCollision(const CAnimatedVolume&, CCollision&, bool) const;
    virtual bool TestCollision(const CWorldVolume&, CCollision&, bool) const;
    virtual bool TestCollision(CVector3, CCollision&, bool) const;
    virtual bool TestCollision(const CLine3&, CCollision&, bool) const;
    virtual bool TestCollision(const CPlane&, CCollision&, bool) const;
    virtual bool TestCollision(const CTriangle&, CCollision&, bool) const;
};

class CVolCapsule : public IVolume {
public:
    virtual ~CVolCapsule();
    virtual IVolume* Create() const;
    virtual void TransformedCopy(const IVolume&, const CMatrix&);

    unsigned char unknown04[8];
    float m_value;
    CLine3 m_line;
};

void CVolCapsule::TransformedCopy(const IVolume& volume, const CMatrix& matrix) {
    const CVolCapsule& other = (const CVolCapsule&)volume;
    CVector3 start;
    CVector3 end;

    m_value = other.m_value;
    m_line = other.m_line;
    matrix.TransformPoint(start, other.m_line.m_start);
    matrix.TransformPoint(end, other.m_line.m_end);
    m_line.SetSE(start, end);
}

IVolume* CVolCapsule::Create() const {
    return new CVolCapsule;
}
