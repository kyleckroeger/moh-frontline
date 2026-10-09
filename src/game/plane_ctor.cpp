// A fragment of plane.cpp (0x80091408): the CPlane constructor from a normal
// (copied as two doubles) and a distance. The file name is this project's;
// the original record is plane.cpp and GetIntersection and Transform before
// it are not reconstructed. CPlane and CVector3 are named by the mangled
// symbols; the 16-byte vector view is inferred.
union CVector3Data {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3Data d;
} __attribute__((aligned(16)));

class CPlane {
public:
    CPlane(CVector3, float);

    CVector3 m_normal;
    float m_d;
} __attribute__((aligned(16)));

CPlane::CPlane(CVector3 normal, float d) : m_normal(normal), m_d(d) {
}
