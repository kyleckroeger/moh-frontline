// A fragment of cdbgeom.cpp (0x8006e058): the CCDBTriangle constructor from
// a database triangle (its three vertices copied from the vertex records the
// triangle points to). The file name is this project's; the original record
// is cdbgeom.cpp. CCDBTriangle, CDBTri and CDBVector are named by the mangled
// symbols; the members and CVector3's inline Set (arguments evaluated right
// to left, as the image's loads show) are inferred views.
/* Inferred: CVector3 as four floats overlaid with two doubles. */
union CVector3Data {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    void Set(float x, float y, float z) {
        d.v[0] = x;
        d.v[1] = y;
        d.v[2] = z;
    }

    CVector3Data d;
} __attribute__((aligned(8)));

/* Inferred: a database vertex and a triangle's three vertex pointers. */
struct CDBVector {
    float x;
    float y;
    float z;
};

struct CDBTri {
    CDBVector* m_verts[3];
};

class CCDBTriangle {
public:
    CCDBTriangle(const CDBTri&);

    CVector3 m_vertices[3];
};

CCDBTriangle::CCDBTriangle(const CDBTri& tri) {
    m_vertices[0].Set(tri.m_verts[0]->x, tri.m_verts[0]->y, tri.m_verts[0]->z);
    m_vertices[1].Set(tri.m_verts[1]->x, tri.m_verts[1]->y, tri.m_verts[1]->z);
    m_vertices[2].Set(tri.m_verts[2]->x, tri.m_verts[2]->y, tri.m_verts[2]->z);
}
