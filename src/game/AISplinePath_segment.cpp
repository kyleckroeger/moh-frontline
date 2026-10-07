// A fragment of AISplinePath.cpp (0x80051b90): CAISplinePathSegment::Expand,
// which evaluates the segment's cubic at t by Horner's rule into the output
// vector (coefficients at +8, +24, +40 and +56), and the segment's empty
// destructor and constructor. The classes are named by the mangled symbols;
// the coefficient layout and the in-place vector operators are inferred.
class CVector3 {
public:
    void operator+=(const CVector3& o) {
        x += o.x;
        y += o.y;
        z += o.z;
    }
    void operator*=(float s) {
        x *= s;
        y *= s;
        z *= s;
    }

    float x;
    float y;
    float z;
    float w;
};

class CAISplinePathSegment {
public:
    CAISplinePathSegment();
    ~CAISplinePathSegment();
    void Expand(float, CVector3&);

    unsigned char unknown00[8];
    CVector3 m_coefficients[4];
};

void CAISplinePathSegment::Expand(float t, CVector3& out) {
    out.x = t * m_coefficients[0].x;
    out.y = t * m_coefficients[0].y;
    out.z = t * m_coefficients[0].z;
    out += m_coefficients[1];
    out *= t;
    out += m_coefficients[2];
    out *= t;
    out += m_coefficients[3];
}

CAISplinePathSegment::~CAISplinePathSegment() {
}

CAISplinePathSegment::CAISplinePathSegment() {
}
