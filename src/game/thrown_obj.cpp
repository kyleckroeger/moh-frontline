// CThrownObject: the weak object-type, cast and script-transform defaults, the
// deleted flag, Destroy (clears the destruction mark and hands the object to
// DeleteSpecialThrownObject), MarkForDestruction (once), SetVelocity (clamped
// to twice the bounding sphere's radius per update when the object has an
// owner) and HandleBulletCollision (a projectile adds its velocity, scaled by
// the definition's factor, through SetVelocity and plays the impact sound
// when enabled). The class and enum names come from the mangled symbols; the
// members and the flag byte are inferred views, the result types are
// inferred, CThrownObject and CStaticObject are non-virtual views, CBullet's
// AsProjectile and GetVelocity are at +320 and +324 of __vt__7CBullet (the
// earlier entries are placeholders named by offset), the vector operators are
// inferred inline helpers, and sqrtf is the MSL inline square root. The
// defaults are inline in the original (weak symbols), so they are defined
// __declspec(weak). CommitUpdate and the rest of the file are not part of
// this unit.
struct VECTOR3VIEW {
    float x;
    float y;
    float z;
} __attribute__((aligned(8)));

class CVector3 {
public:
    CVector3& operator*=(float f) {
        v.x *= f;
        v.y *= f;
        v.z *= f;
        return *this;
    }
    CVector3& operator+=(const CVector3& o) {
        v.x += o.v.x;
        v.y += o.v.y;
        v.z += o.v.z;
        return *this;
    }

    VECTOR3VIEW v;
};

class CCollision;
class CProjectileBullet;

class CBullet {
public:
    virtual void unknown008();
    virtual void unknown00c();
    virtual void unknown010();
    virtual void unknown014();
    virtual void unknown018();
    virtual void unknown01c();
    virtual void unknown020();
    virtual void unknown024();
    virtual void unknown028();
    virtual void unknown02c();
    virtual void unknown030();
    virtual void unknown034();
    virtual void unknown038();
    virtual void unknown03c();
    virtual void unknown040();
    virtual void unknown044();
    virtual void unknown048();
    virtual void unknown04c();
    virtual void unknown050();
    virtual void unknown054();
    virtual void unknown058();
    virtual void unknown05c();
    virtual void unknown060();
    virtual void unknown064();
    virtual void unknown068();
    virtual void unknown06c();
    virtual void unknown070();
    virtual void unknown074();
    virtual void unknown078();
    virtual void unknown07c();
    virtual void unknown080();
    virtual void unknown084();
    virtual void unknown088();
    virtual void unknown08c();
    virtual void unknown090();
    virtual void unknown094();
    virtual void unknown098();
    virtual void unknown09c();
    virtual void unknown0a0();
    virtual void unknown0a4();
    virtual void unknown0a8();
    virtual void unknown0ac();
    virtual void unknown0b0();
    virtual void unknown0b4();
    virtual void unknown0b8();
    virtual void unknown0bc();
    virtual void unknown0c0();
    virtual void unknown0c4();
    virtual void unknown0c8();
    virtual void unknown0cc();
    virtual void unknown0d0();
    virtual void unknown0d4();
    virtual void unknown0d8();
    virtual void unknown0dc();
    virtual void unknown0e0();
    virtual void unknown0e4();
    virtual void unknown0e8();
    virtual void unknown0ec();
    virtual void unknown0f0();
    virtual void unknown0f4();
    virtual void unknown0f8();
    virtual void unknown0fc();
    virtual void unknown100();
    virtual void unknown104();
    virtual void unknown108();
    virtual void unknown10c();
    virtual void unknown110();
    virtual void unknown114();
    virtual void unknown118();
    virtual void unknown11c();
    virtual void unknown120();
    virtual void unknown124();
    virtual void unknown128();
    virtual void unknown12c();
    virtual void unknown130();
    virtual void unknown134();
    virtual void unknown138();
    virtual void unknown13c();
    virtual CProjectileBullet* AsProjectile();
    virtual void GetVelocity(CVector3&);
};

struct HITVIEW {
    unsigned char unknown00[16];
    CVector3 point;
};

struct COLLISIONVIEW {
    unsigned char unknown00[24];
    HITVIEW* hit;
};

struct THROWNPROPSVIEW {
    unsigned char unknown00[100];
    float impulseScale;
};

struct THROWNDEFVIEW {
    unsigned char unknown0[8];
    THROWNPROPSVIEW* properties;
};

void PlayImpactSound(unsigned short, unsigned short, CVector3, bool);

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
    void HandleBulletCollision(CBullet*, const CCollision&);
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
    unsigned char unknown298[48];
    THROWNDEFVIEW* m_definition;
    unsigned char unknown2CC[40];
    unsigned char flag0 : 1;
    unsigned char flag1 : 1;
    unsigned char flag2 : 1;
    unsigned char flag3 : 1;
    unsigned char deleted : 1;
    unsigned char markedForDestruction : 1;
    unsigned char flag6 : 1;
    unsigned char flag7 : 1;
    unsigned char unknown2F5[3];
    int m_holder;
    int m_impactSound;
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

void CThrownObject::HandleBulletCollision(CBullet* bullet, const CCollision& collision) {
    if (bullet->AsProjectile() && m_holder == -1) {
        CVector3 impulse;
        HITVIEW* hit;

        bullet->GetVelocity(impulse);
        impulse *= m_definition->properties->impulseScale;
        m_velocity += impulse;
        SetVelocity(m_velocity);
        hit = ((const COLLISIONVIEW&)collision).hit;
        if (m_impactSound == 1)
            PlayImpactSound(15, 1, hit->point, false);
    }
}
