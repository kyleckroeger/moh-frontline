// CThrownObject: the weak object-type, cast and script-transform defaults, the
// deleted flag, Destroy (clears the destruction mark and hands the object to
// DeleteSpecialThrownObject), MarkForDestruction (once) and SetVelocity
// (clamped to twice the bounding sphere's radius per update when the object
// has an owner). The class and enum names come from the mangled symbols; the
// members and the flag byte are inferred views, the result types are
// inferred, the classes are non-virtual views, and sqrtf is the MSL inline
// square root. The defaults are inline in the original (weak symbols), so
// they are defined __declspec(weak). HandleBulletCollision and the rest of
// the file are not part of this unit.
struct VECTOR3VIEW {
    float x;
    float y;
    float z;
} __attribute__((aligned(8)));

class CVector3 {
public:
    VECTOR3VIEW v;
};

class CVolSphere {
public:
    float GetRadius() const;
};

extern inline float sqrtf(float x)
{
    const double _half = .5;
    const double _three = 3.0;
    volatile float y;
    if (x > 0.0f)
    {
        double guess = __frsqrte((double)x);
        guess = _half*guess*(_three - guess*guess*x);
        guess = _half*guess*(_three - guess*guess*x);
        guess = _half*guess*(_three - guess*guess*x);
        y = (float)(x*guess);
        return y ;
    }
    return x;
}
enum EBSEventEnum {};

class CThrownObject;

void DeleteSpecialThrownObject(CThrownObject*);

class CStaticObject {
public:
    void MarkForDestruction(int);
};

class CThrownObject : public CStaticObject {
public:
    void Destroy();
    void SetVelocity(CVector3&);
    void MarkForDestruction(int);
    int GetObjectType() const;
    CThrownObject* AsThrownObject();
    void TranslateFromScript(CVector3&, CVector3&, EBSEventEnum);
    void RotateFromScript(CVector3&, CVector3&, EBSEventEnum);
    void ScaleFromScript(float, float, EBSEventEnum);
    void SetDeleted(bool);

    unsigned char unknown000[268];
    CVolSphere* m_sphere;
    unsigned char unknown110[80];
    CVector3 m_velocity;
    unsigned char unknown170[108];
    int m_owner;
    unsigned char unknown1E0[168];
    CVector3 m_lastVelocity;
    unsigned char unknown298[92];
    unsigned char flag0 : 1;
    unsigned char flag1 : 1;
    unsigned char flag2 : 1;
    unsigned char flag3 : 1;
    unsigned char deleted : 1;
    unsigned char markedForDestruction : 1;
    unsigned char flag6 : 1;
    unsigned char flag7 : 1;
};

__declspec(weak) int CThrownObject::GetObjectType() const {
    return 4;
}

__declspec(weak) CThrownObject* CThrownObject::AsThrownObject() {
    return this;
}

__declspec(weak) void CThrownObject::TranslateFromScript(CVector3&, CVector3&, EBSEventEnum) {
}

__declspec(weak) void CThrownObject::RotateFromScript(CVector3&, CVector3&, EBSEventEnum) {
}

__declspec(weak) void CThrownObject::ScaleFromScript(float, float, EBSEventEnum) {
}

void CThrownObject::SetDeleted(bool deletedFlag) {
    deleted = deletedFlag;
}

void CThrownObject::Destroy() {
    markedForDestruction = 0;
    DeleteSpecialThrownObject(this);
}

void CThrownObject::MarkForDestruction(int when) {
    if (!markedForDestruction) {
        CStaticObject::MarkForDestruction(when);
        markedForDestruction = 1;
    }
}

void CThrownObject::SetVelocity(CVector3& velocity) {
    m_velocity = velocity;
    if (m_owner != -1) {
        CVolSphere* sphere = m_sphere;
        float speed = sqrtf(m_velocity.v.x * m_velocity.v.x + m_velocity.v.y * m_velocity.v.y + m_velocity.v.z * m_velocity.v.z);
        float limit = 2.0f * sphere->GetRadius();

        if (limit < speed) {
            float scale = limit / speed;

            m_velocity.v.x *= scale;
            m_velocity.v.y *= scale;
            m_velocity.v.z *= scale;
        }
    }
    m_lastVelocity = m_velocity;
}
