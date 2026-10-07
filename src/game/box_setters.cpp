// A fragment of box.cpp (0x800c514c): CVolBox's depth and width (twice the
// half extents), SetCorners (the centre midway between the corners, each half
// extent the absolute projection of the centre's offset from the first
// corner on a basis row) and the shape setters. The depth, height and width
// setters store half their argument; SetBasis and SetCenter copy whole
// vectors.
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
    float GetDepth() const;
    float GetWidth() const;
    void SetCorners(CVector3, CVector3);
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

float CVolBox::GetDepth() const {
    return 2.0f * m_halfDepth;
}

float CVolBox::GetWidth() const {
    return 2.0f * m_halfWidth;
}

void CVolBox::SetCorners(CVector3 a, CVector3 b) {
    m_center.v.x = b.v.x + a.v.x;
    m_center.v.y = b.v.y + a.v.y;
    m_center.v.z = b.v.z + a.v.z;
    m_center.v.x *= 0.5f;
    m_center.v.y *= 0.5f;
    m_center.v.z *= 0.5f;
    float dx = m_center.v.x - a.v.x;
    float dy = m_center.v.y - a.v.y;
    float dz = m_center.v.z - a.v.z;
    float t = m_basis[2].v.x * dx + m_basis[2].v.y * dy + m_basis[2].v.z * dz;
    float h = m_basis[1].v.x * dx + m_basis[1].v.y * dy + m_basis[1].v.z * dz;
    float w = m_basis[0].v.x * dx + m_basis[0].v.y * dy + m_basis[0].v.z * dz;
    double fw = __fabs(w);
    double fh = __fabs(h);
    double ft = __fabs(t);
    m_halfWidth = fw;
    m_halfDepth = fh;
    m_halfHeight = ft;
}

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
