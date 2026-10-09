// A fragment of cdbgeom.cpp (0x8006f738): the CCDBVolBox constructor from a
// database box record. The three basis vectors and the centre are built from
// the record's floats, the basis and centre are set, then the width, depth
// and height. The file name is this project's; the original record is
// cdbgeom.cpp. The classes and functions are named by the mangled symbols;
// the record layout (three extents, a 3x3 basis, the centre) and the vector
// view (built from three floats, passed by value as two doubles) are inferred. CCDBVolBox declares its
// destructor (defined elsewhere) first so its table stays elsewhere.
struct VECTOR3VIEW {
    float x;
    float y;
    float z;
} __attribute__((aligned(8)));

class CVector3 {
public:
    CVector3() {}
    CVector3(const float* v) : x(v[0]), y(v[1]), z(v[2]) {}
    CVector3(const CVector3& o) {
        ((double*)this)[0] = ((const double*)&o)[0];
        ((double*)this)[1] = ((const double*)&o)[1];
    }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

/* Inferred: the database box record. */
struct CDBBox {
    float width;
    float depth;
    float height;
    float basis[3][3];
    float center[3];
};

class IVolume {
public:
    virtual ~IVolume();
};

class CVolBox : public IVolume {
public:
    CVolBox() {}
    virtual ~CVolBox();
    void SetBasis(CVector3, CVector3, CVector3);
    void SetCenter(CVector3);
    void SetWidth(float);
    void SetDepth(float);
    void SetHeight(float);

    float m_halfWidth;
    float m_halfDepth;
    float m_halfHeight;
    VECTOR3VIEW m_basis[3];
    VECTOR3VIEW m_center;
};

class CCDBVolBox : public CVolBox {
public:
    virtual ~CCDBVolBox();
    CCDBVolBox(const CDBBox&);
};

CCDBVolBox::CCDBVolBox(const CDBBox& box) {
    CVector3 right(box.basis[0]);
    CVector3 forward(box.basis[1]);
    CVector3 up(box.basis[2]);
    CVector3 center(box.center);
    SetBasis(right, forward, up);
    SetCenter(center);
    SetWidth(box.width);
    SetDepth(box.depth);
    SetHeight(box.height);
}
