// A fragment of box.cpp (0x800c52d0): CVolBox::TransformedCopy (copies the
// three floats, transforms three vectors as directions and the last as a
// point by the matrix), CVolBox::Create allocates a new,
// default-constructed box (80 bytes) through DWI_alloc (no name, flag 1024;
// the inline class operator new is inferred from the call; the inline
// constructors set the IVolume and CVolBox virtual tables), and the
// assignment operator (unless assigning to itself: three floats and four
// 16-byte vectors). IVolume and CVolBox are named by the mangled symbols;
// IVolume's virtual functions are declared in the order of __vt__7IVolume,
// CVolBox declares its destructor first (defined elsewhere, so its virtual
// table is not emitted here), and its members are inferred from the copy
// (their meaning is not known). The rest of the file is not part of this
// unit.
void* DWI_alloc(const char*, int, int);

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
    void TransformVector(CVector3&, CVector3) const;
    void TransformPoint(CVector3&, CVector3) const;
};
class CTriangle;
class CPlane;
class CLine3;
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

class CVolBox : public IVolume {
public:
    virtual ~CVolBox();
    virtual IVolume* Create() const;
    virtual void TransformedCopy(const IVolume&, const CMatrix&);

    static void* operator new(unsigned long size) { return DWI_alloc(0, size, 1024); }

    CVolBox& operator=(const CVolBox&);

    float m_values[3];
    CVector3 m_vectors[4];
};

void CVolBox::TransformedCopy(const IVolume& volume, const CMatrix& matrix) {
    const CVolBox& other = (const CVolBox&)volume;

    m_values[0] = other.m_values[0];
    m_values[1] = other.m_values[1];
    m_values[2] = other.m_values[2];
    matrix.TransformVector(m_vectors[0], other.m_vectors[0]);
    matrix.TransformVector(m_vectors[1], other.m_vectors[1]);
    matrix.TransformVector(m_vectors[2], other.m_vectors[2]);
    matrix.TransformPoint(m_vectors[3], other.m_vectors[3]);
}

IVolume* CVolBox::Create() const {
    return new CVolBox;
}

CVolBox& CVolBox::operator=(const CVolBox& other) {
    if (&other != this) {
        m_values[0] = other.m_values[0];
        m_values[1] = other.m_values[1];
        m_values[2] = other.m_values[2];
        m_vectors[0] = other.m_vectors[0];
        m_vectors[1] = other.m_vectors[1];
        m_vectors[2] = other.m_vectors[2];
        m_vectors[3] = other.m_vectors[3];
    }
    return *this;
}
