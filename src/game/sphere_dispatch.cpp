// A fragment of sphere.cpp (0x800cb4f0): CVolSphere's double-dispatch
// collision tests. Against CDB objects, capsules, world volumes and CSG
// volumes the collision order is swapped and the other volume tests against
// the sphere; against a fiber the collision's line flag is set and the sphere
// tests the fiber's line; an animated volume is tested as a sphere. The
// sphere-sphere test follows: the direction between the centres is
// normalised (the SDK's inline sqrtf), the separation is the distance less
// both radii, and the result is 1 beyond CONTACT_EPSILON, 3 within
// -CONTACT_EPSILON and 2 between, when a contact (the direction, the point on
// this sphere's surface, the separation, index 0) is added. The file name is
// this project's; the original record is sphere.cpp (sphere.cpp holds the
// box and generic tests onward). The classes come from the mangled symbols; IVolume's
// virtual functions are declared in the order of __vt__7IVolume, CVolSphere
// redeclares them (its destructor first, defined elsewhere, so its virtual
// table is not emitted here), CAnimatedVolume is taken to derive from
// CVolSphere (the call it makes implies it), and the collision flag is an
// inferred bit-field view. The sphere members (radius at +12, centre at +16),
// the collision result at +20, the contact record and the int-sized results
// are inferred.
#include <math.h>

class CMatrix;

// Inferred: CVector3 as four floats overlaid with two doubles (its copies
// move doubleword pairs).
union CVector3Data {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3Data d;
} __attribute__((aligned(8)));

// Inferred contact record view: two vectors, the separation and an index.
struct SClsnContact {
    CVector3 m_vector0;
    CVector3 m_vector1;
    float m_separation;
    int m_index;
} __attribute__((aligned(16)));

extern const float CONTACT_EPSILON;
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

class CCollision {
public:
    void SwapOrder();
    void AddContact(const SClsnContact&);

    unsigned char unknown00[16];
    unsigned char m_line : 1;
    unsigned char unknown10 : 7;
    unsigned char unknown11[3];
    int m_result;
};

class IVolume {
public:
    virtual ~IVolume();
    virtual IVolume* Create() const;
    virtual void TransformedCopy(const IVolume&, const CMatrix&);
    virtual int TestCollision(const IVolume&, CCollision&, bool) const;
    virtual int TestCollision(const CVolBox&, CCollision&, bool) const;
    virtual int TestCollision(const CVolSphere&, CCollision&, bool) const;
    virtual int TestCollision(const CVolCapsule&, CCollision&, bool) const;
    virtual int TestCollision(const CCDBObject&, CCollision&, bool) const;
    virtual int TestCollision(const CVolFiber&, CCollision&, bool) const;
    virtual int TestCollision(const CAnimatedVolume&, CCollision&, bool) const;
    virtual int TestCollision(const CWorldVolume&, CCollision&, bool) const;
    virtual int TestCollision(CVector3, CCollision&, bool) const;
    virtual int TestCollision(const CLine3&, CCollision&, bool) const;
    virtual int TestCollision(const CPlane&, CCollision&, bool) const;
    virtual int TestCollision(const CTriangle&, CCollision&, bool) const;
};

class CCDBObject : public IVolume {};
class CVolCapsule : public IVolume {};
class CWorldVolume : public IVolume {};
class CCSGVolume : public IVolume {};

class CVolFiber : public IVolume {
public:
    const CLine3& GetLine() const;
};

class CVolSphere : public IVolume {
public:
    virtual ~CVolSphere();
    virtual IVolume* Create() const;
    virtual void TransformedCopy(const IVolume&, const CMatrix&);
    virtual int TestCollision(const IVolume&, CCollision&, bool) const;
    virtual int TestCollision(const CVolBox&, CCollision&, bool) const;
    virtual int TestCollision(const CVolSphere&, CCollision&, bool) const;
    virtual int TestCollision(const CVolCapsule&, CCollision&, bool) const;
    virtual int TestCollision(const CCDBObject&, CCollision&, bool) const;
    virtual int TestCollision(const CVolFiber&, CCollision&, bool) const;
    virtual int TestCollision(const CAnimatedVolume&, CCollision&, bool) const;
    virtual int TestCollision(const CWorldVolume&, CCollision&, bool) const;
    virtual int TestCollision(CVector3, CCollision&, bool) const;
    virtual int TestCollision(const CLine3&, CCollision&, bool) const;
    virtual int TestCollision(const CPlane&, CCollision&, bool) const;
    virtual int TestCollision(const CTriangle&, CCollision&, bool) const;
    int TestCollision(const CCSGVolume&, CCollision&, bool) const;

    unsigned char unknown04[8];
    float m_radius;
    CVector3 m_center;
};

class CAnimatedVolume : public CVolSphere {};

int CVolSphere::TestCollision(const CCDBObject& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    return other.TestCollision(*this, collision, flag);
}

int CVolSphere::TestCollision(const CVolFiber& other, CCollision& collision, bool flag) const {
    collision.m_line = 1;
    return TestCollision(other.GetLine(), collision, flag);
}

int CVolSphere::TestCollision(const CVolCapsule& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    return other.TestCollision(*this, collision, flag);
}

int CVolSphere::TestCollision(const CWorldVolume& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    return other.TestCollision(*this, collision, flag);
}

int CVolSphere::TestCollision(const CCSGVolume& other, CCollision& collision, bool flag) const {
    collision.SwapOrder();
    return other.TestCollision(*this, collision, flag);
}

int CVolSphere::TestCollision(const CAnimatedVolume& other, CCollision& collision, bool flag) const {
    return TestCollision((const CVolSphere&)other, collision, flag);
}

int CVolSphere::TestCollision(const CVolSphere& other, CCollision& collision, bool flag) const {
    CVector3 direction;
    direction.d.v[0] = other.m_center.d.v[0] - m_center.d.v[0];
    direction.d.v[1] = other.m_center.d.v[1] - m_center.d.v[1];
    direction.d.v[2] = other.m_center.d.v[2] - m_center.d.v[2];
    float distance = sqrtf(direction.d.v[0] * direction.d.v[0] + direction.d.v[1] * direction.d.v[1] +
                           direction.d.v[2] * direction.d.v[2]);
    if (distance != 0.0f) {
        direction.d.v[0] *= 1.0f / distance;
        direction.d.v[1] *= 1.0f / distance;
        direction.d.v[2] *= 1.0f / distance;
    }
    float separation = distance - (m_radius + other.m_radius);
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
            SClsnContact contact;
            contact.m_separation = separation;
            float radius = m_radius;
            contact.m_vector0 = direction;
            direction.d.v[0] *= radius;
            direction.d.v[1] *= radius;
            direction.d.v[2] *= radius;
            contact.m_vector1.d.v[0] = m_center.d.v[0] + direction.d.v[0];
            contact.m_vector1.d.v[1] = m_center.d.v[1] + direction.d.v[1];
            contact.m_vector1.d.v[2] = m_center.d.v[2] + direction.d.v[2];
            contact.m_index = 0;
            collision.AddContact(contact);
        }
    }
    return result;
}
