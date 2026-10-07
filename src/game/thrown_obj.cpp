// CThrownObject: the weak object-type, cast and script-transform defaults, the
// deleted flag, Destroy (clears the destruction mark and hands the object to
// DeleteSpecialThrownObject), MarkForDestruction (once), SetVelocity (clamped
// to twice the bounding sphere's radius per update when the object has an
// owner) and HandleBulletCollision (a projectile adds its velocity, scaled by
// the definition's factor, through SetVelocity and plays the impact sound
// when enabled) and CommitUpdate (while alive: register the volume with a new
// collider, constrain the velocity, keep the position and velocity, and
// update the flag bits; otherwise mark for destruction) and AttemptUpdate
// (gravity on the z velocity while falling, the clamp, the step from the
// saved position, then the sorted bounds from the collider's extent or the
// position), and BeginUpdate (light volumes, saved state, proximity triggers,
// a pending impact sound, and the lifetime countdown by whole ticks while
// held) and Draw (above the definition's minimum speed, roll the orientation
// about the horizontal axis normal to the velocity by the distance moved over
// the sphere's radius; then draw the mesh at the orientation and transform,
// with its light volume). The
// class and enum
// names come from the mangled symbols; the members and the flag byte are
// inferred views, the result types are inferred, ISceneNode's virtual
// functions follow __vt__13CThrownObject (CThrownObject declares its
// destructor first, so its vtable is not emitted here), the collider view
// (virtual entries at +16 and +72 used, the rest placeholders), the bounds
// entries and the state, clamp and bounds helpers are inferred, CBullet's
// AsProjectile and GetVelocity are at +320 and +324 of __vt__7CBullet (the
// earlier entries are placeholders named by offset), the vector operators are
// inferred inline helpers, and sqrtf is the MSL inline square root. The
// defaults are inline in the original (weak symbols), so they are defined
// __declspec(weak). CStaticMesh's virtual functions follow
// __vt__11CStaticMesh; the draw context's matrix pointer, the vector helpers
// and the quaternion layout are inferred. ConstrainVelocity and the rest of
// the file are not part of this unit.
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

struct VECTOR3VIEW {
    float x;
    float y;
    float z;
} __attribute__((aligned(8)));

class CVector3 {
public:
    float LengthSq() const { return v.x * v.x + v.y * v.y + v.z * v.z; }
    void Normalize() {
        float length = sqrtf(LengthSq());
        if (length != 0.0f) {
            float inverse = 1.0f / length;
            v.x *= inverse;
            v.y *= inverse;
            v.z *= inverse;
        }
    }
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
    unsigned char unknown00[68];
    float minRollSpeed;
    unsigned char unknown48[28];
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

enum EBSEventEnum {};

class CThrownObject;

void DeleteSpecialThrownObject(CThrownObject*);

class CDrawContext;
struct BPDLightVolume;

class CLightVolumeManager {
public:
    void Update(float);
    BPDLightVolume* GetVolume();

    unsigned char unknown00[32];
};

class BSGO_Basic {
    unsigned char unknown00[36];
};

void ForceUpdateProximityTriggerStatus(BSGO_Basic*, bool);

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    void SetPos(CVector3);
    void BuildRot(CVector3, float);
    void Multiply(const CMatrix&, const CMatrix&);

    unsigned char rows[48];
    CVector3 position;
    static bool s_ClassInit;
} __attribute__((aligned(16)));

class CQuaternion {
public:
    void SetFromMatrix(const CMatrix&);
    void Multiply(CQuaternion);
    void GetMatrix(CMatrix&) const;

    float x;
    float y;
    float z;
    float w;
};

struct BPDLightVolume;

class CStaticMesh {
public:
    virtual ~CStaticMesh();
    virtual void Draw(CDrawContext&, int);
    virtual void EnableLighting(bool);
    virtual bool IsLightingEnabled();
    static void SetLightVolume(BPDLightVolume*);
};

/* inferred: the draw context's current-matrix pointer at +192 */
struct DRAWCONTEXTVIEW {
    unsigned char unknown000[192];
    CMatrix* matrix;
};

/* inferred: an entry of a sorted bounds list, keyed by the float at +8 */
struct BOUNDVIEW {
    unsigned char unknown0[8];
    float key;
};

/* inferred: the object the thrown object's volume is registered with */
class COLLIDERVIEW {
public:
    virtual void unknown08();
    virtual void unknown0c();
    virtual void unknown10(CVolSphere*, CMatrix*);
    virtual void unknown14();
    virtual void unknown18();
    virtual void unknown1c();
    virtual void unknown20();
    virtual void unknown24();
    virtual void unknown28();
    virtual void unknown2c();
    virtual void unknown30();
    virtual void unknown34();
    virtual void unknown38();
    virtual void unknown3c();
    virtual void unknown40();
    virtual void unknown44();
    virtual void unknown48(CVector3&, CVector3&);
};

class ISceneNode {
public:
    virtual void MarkForDestruction(int);
    virtual ~ISceneNode();
    virtual void Destroy();
    virtual void BeginUpdate(float);
    virtual void UpdateAI(float);
    virtual void CommitAI();
    virtual void ConstrainVelocity();
    virtual void AttemptUpdate(float);
    virtual void OnCollision(const CCollision&);
    virtual void CommitUpdate();
    virtual void Draw(CDrawContext&);
    virtual void GetLocalBoundingVolume();
    virtual void GetWorldBoundingVolume();
    virtual void GetTMLocalToWorld(CMatrix&) const;
    virtual void GetPosition(CVector3&) const;
};

class CStaticObject : public ISceneNode {
public:
    void MarkForDestruction(int);
};

class CThrownObject : public CStaticObject {
public:
    virtual ~CThrownObject();
    void Destroy();
    void CommitUpdate();
    void AttemptUpdate(float);
    void BeginUpdate(float);
    void Draw(CDrawContext&);
    void SetBounds(const CVector3& low, const CVector3& high) {
        m_bounds[0]->key = low.v.x;
        m_bounds[2]->key = low.v.y;
        m_bounds[1]->key = high.v.x;
        m_bounds[3]->key = high.v.y;
    }
    CVector3 PositionRow() const { return m_tm.position; }
    void SaveState() {
        m_savedPosition = PositionRow();
        m_lastVelocity = m_velocity;
    }
    void SetVelocity(CVector3&);
    void ClampVelocity() {
        CVolSphere* sphere = m_sphere;
        if (m_owner != -1) {
            float speed = sqrtf(m_velocity.v.x * m_velocity.v.x + m_velocity.v.y * m_velocity.v.y + m_velocity.v.z * m_velocity.v.z);
            float limit = 2.0f * sphere->GetRadius();

            if (limit < speed) {
                float scale = limit / speed;

                m_velocity.v.x *= scale;
                m_velocity.v.y *= scale;
                m_velocity.v.z *= scale;
            }
        }
    }
    void HandleBulletCollision(CBullet*, const CCollision&);
    void MarkForDestruction(int);
    int GetObjectType() const;
    CThrownObject* AsThrownObject();
    void TranslateFromScript(CVector3&, CVector3&, EBSEventEnum);
    void RotateFromScript(CVector3&, CVector3&, EBSEventEnum);
    void ScaleFromScript(float, float, EBSEventEnum);
    void SetDeleted(bool);

    unsigned char unknown004[8];
    BOUNDVIEW* m_bounds[4];
    unsigned char unknown01C[20];
    CStaticMesh* m_mesh;
    unsigned char unknown034[12];
    CMatrix m_tm;
    unsigned char unknown080[140];
    CVolSphere* m_sphere;
    COLLIDERVIEW* m_collider;
    COLLIDERVIEW* m_lastCollider;
    unsigned char unknown118[8];
    CVector3 m_savedPosition;
    unsigned char unknown130[48];
    CVector3 m_velocity;
    unsigned char unknown170[108];
    int m_owner;
    unsigned char unknown1E0[52];
    int m_drawMode;
    unsigned char unknown218[72];
    CLightVolumeManager m_lightVolumes;
    float m_lifetime;
    unsigned char unknown284[4];
    CVector3 m_lastVelocity;
    CVector3 m_force;
    CVector3 m_step;
    CQuaternion m_orientation;
    THROWNDEFVIEW* m_definition;
    int m_pendingSound;
    BSGO_Basic m_scriptObject;
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

void CThrownObject::CommitUpdate() {
    if (!deleted) {
        if (m_lifetime > 0.0f) {
            if (m_collider != m_lastCollider && m_collider && m_sphere)
                m_collider->unknown10(m_sphere, &m_tm);
            ConstrainVelocity();
            SaveState();
            if (flag1)
                flag0 = 0;
            else
                flag0 = 1;
            flag1 = 0;
            flag2 = 0;
        } else {
            MarkForDestruction(0);
        }
    }
}

void CThrownObject::AttemptUpdate(float dt) {
    if (!deleted) {
        CVector3 pos;
        CVector3 low;
        CVector3 high;
        CVector3 position;

        m_force.v.z = 0.0f;
        m_force.v.y = 0.0f;
        m_force.v.x = 0.0f;
        if (flag0 && m_holder)
            m_velocity.v.z = -0.00545535097f * dt + m_lastVelocity.v.z;
        ClampVelocity();
        m_step.v.x = dt * m_velocity.v.x;
        m_step.v.y = dt * m_velocity.v.y;
        m_step.v.z = dt * m_velocity.v.z;
        pos.v.x = m_savedPosition.v.x + m_step.v.x;
        pos.v.y = m_savedPosition.v.y + m_step.v.y;
        pos.v.z = m_savedPosition.v.z + m_step.v.z;
        m_tm.SetPos(pos);
        if (m_lastCollider) {
            m_lastCollider->unknown10(m_sphere, &m_tm);
            m_lastCollider->unknown48(low, high);
            SetBounds(low, high);
        } else {
            GetPosition(position);
            SetBounds(position, position);
        }
    }
}

void CThrownObject::BeginUpdate(float dt) {
    m_lightVolumes.Update(dt);
    m_savedPosition = PositionRow();
    m_lastVelocity = m_velocity;
    ForceUpdateProximityTriggerStatus(&m_scriptObject, true);
    if (m_pendingSound > 0) {
        if (m_impactSound == 1)
            PlayImpactSound(m_pendingSound, 4, m_savedPosition, false);
        m_pendingSound = -1;
    }
    if (m_lifetime > 0.0f && m_holder) {
        m_lifetime -= (int)dt;
        if (m_lifetime < 0.0f)
            m_lifetime = 0.0f;
    }
}

void CThrownObject::Draw(CDrawContext& context) {
    CMatrix rotation;

    float minSpeed = m_definition->properties->minRollSpeed;

    if (sqrtf(m_velocity.LengthSq()) > minSpeed) {
        CVector3 up;
        CVector3 axis;
        CVolSphere* sphere = m_sphere;
        float radius;
        CQuaternion turn;

        up.v.x = 0.0f;
        up.v.y = 0.0f;
        up.v.z = 1.0f;
        axis.v.x = up.v.y * m_velocity.v.z - up.v.z * m_velocity.v.y;
        axis.v.y = up.v.z * m_velocity.v.x - up.v.x * m_velocity.v.z;
        axis.v.z = up.v.x * m_velocity.v.y - up.v.y * m_velocity.v.x;
        axis.Normalize();
        if (sphere)
            radius = sphere->GetRadius();
        else
            radius = 1.0f;
        rotation.BuildRot(axis, sqrtf(m_step.LengthSq()) / radius);
        turn.SetFromMatrix(rotation);
        m_orientation.Multiply(turn);
    }
    m_orientation.GetMatrix(rotation);
    rotation.Multiply(rotation, m_tm);
    ((DRAWCONTEXTVIEW&)context).matrix = &rotation;
    if (m_mesh->IsLightingEnabled())
        CStaticMesh::SetLightVolume(m_lightVolumes.GetVolume());
    m_mesh->Draw(context, m_drawMode);
}
