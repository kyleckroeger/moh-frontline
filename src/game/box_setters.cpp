// A fragment of box.cpp (0x800c5258): CVolBox's shape setters. The depth,
// height and width setters store half their argument; SetBasis and SetCenter
// copy whole vectors. SetCorners before them is not part of the unit.
// CVolBox, IVolume and CVector3 are named by the mangled symbols; the member
// layout (half extents at +4, basis rows at +16, centre at +64) is inferred
// from offsets. CVector3 is a view: its doubleword copies need an 8-byte
// aligned three-float body.
class IVolume {
public:
    virtual ~IVolume();
};

struct VECTOR3VIEW {
    float x;
    float y;
    float z;
} __attribute__((aligned(8)));

class CVector3 {
public:
    VECTOR3VIEW v;
};

class CVolBox : public IVolume {
public:
    virtual ~CVolBox();
    void SetDepth(float);
    void SetHeight(float);
    void SetWidth(float);
    void SetBasis(CVector3, CVector3, CVector3);
    void SetCenter(CVector3);

    float m_halfWidth;
    float m_halfDepth;
    float m_halfHeight;
    CVector3 m_basis[3];
    CVector3 m_center;
};

void CVolBox::SetDepth(float depth) {
    m_halfDepth = 0.5f * depth;
}

void CVolBox::SetHeight(float height) {
    m_halfHeight = 0.5f * height;
}

void CVolBox::SetWidth(float width) {
    m_halfWidth = 0.5f * width;
}

void CVolBox::SetBasis(CVector3 x, CVector3 y, CVector3 z) {
    m_basis[0] = x;
    m_basis[1] = y;
    m_basis[2] = z;
}

void CVolBox::SetCenter(CVector3 center) {
    m_center = center;
}
