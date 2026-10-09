// A fragment of cdbgeom.cpp (0x8006c614): CCDBTriangle::TestCollision
// against a sphere (the triangle's nearest point to the centre; the distance
// beyond the radius gives 1 (apart) above CONTACT_EPSILON, 3 below its
// negative and 2 within it; on request the result is raised into the
// collision and a touching contact recorded: the unit direction from the
// point to the centre, the point, the separation and the triangle). The file
// name is this project's; the original record is cdbgeom.cpp. The classes and
// functions are named by the mangled symbols; CVector3 is the double-pair
// view with a copy constructor (the call temporaries are built in place, as
// in the image), the contact view follows capsule_dispatch.cpp with the
// triangle at +36 (as decal_reset.cpp reads it), and the collision's result
// at +20 is inferred. The square root is the libc inline sqrtf. The
// constants link to their pool entries.
#include <math.h>

/* Inferred: CVector3 as four floats overlaid with two doubles. */
union CVector3Data {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3() {}
    CVector3(const CVector3& v) : d(v.d) {}
    CVector3Data d;
} __attribute__((aligned(8)));

class CDBTri;
class IVolume;

struct SClsnContact {
    CVector3 m_vector0;
    CVector3 m_vector1;
    float m_separation;
    const CDBTri* m_tri;
    const IVolume* m_object;
} __attribute__((aligned(16)));

class CCollision {
public:
    void AddContact(const SClsnContact&);

    unsigned char unknown00[20];
    int m_result;
};

class CVolSphere {
public:
    float GetRadius() const;
    CVector3 GetCenter() const;
};

extern const float CONTACT_EPSILON;

class CCDBTriangle {
public:
    CVector3 GetNearestPoint(CVector3, float*, float*) const;
    int TestCollision(const CVolSphere&, CCollision&, const CDBTri*, bool) const;
};

int CCDBTriangle::TestCollision(const CVolSphere& sphere, CCollision& collision, const CDBTri* tri, bool flag) const {
    CVector3 nearest = GetNearestPoint(sphere.GetCenter(), 0, 0);
    const CVector3& center = sphere.GetCenter();
    float dx = center.d.v[0] - nearest.d.v[0];
    float dy = center.d.v[1] - nearest.d.v[1];
    float dz = center.d.v[2] - nearest.d.v[2];
    float distance = sqrtf(dx * dx + dy * dy + dz * dz);
    float separation = distance - sphere.GetRadius();
    int result;
    if (separation > CONTACT_EPSILON)
        result = 1;
    else if (separation < -CONTACT_EPSILON)
        result = 3;
    else
        result = 2;
    if (flag) {
        if (result > collision.m_result)
            collision.m_result = result;
        if (result == 2) {
            float scale = 1.0f / distance;
            SClsnContact contact;
            contact.m_vector0.d.v[0] = scale * dx;
            contact.m_vector0.d.v[1] = scale * dy;
            contact.m_vector0.d.v[2] = scale * dz;
            contact.m_vector1 = nearest;
            contact.m_separation = separation;
            contact.m_tri = tri;
            collision.AddContact(contact);
        }
    }
    return result;
}
