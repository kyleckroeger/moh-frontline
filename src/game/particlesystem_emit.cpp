// A fragment of particlesystem.cpp (0x8007f0ac): CPropertyParticleSystem's
// empty SetLocalToWorld, SetSeed and the particle acceleration, velocity and
// position getters (each vector set from the particle-property record through
// an inline three-float setter, which explains the reversed loads), and
// GetParticleSize (two vectors with a zero z, 0.0f being an entry of the
// file's .sdata2 pool, and a fifth value). The file name is this project's;
// the original record is particlesystem.cpp. The class names come from the
// mangled symbols; the members, the property-record view and the setter are
// inferred (what the size values mean is unknown), and the class is a
// non-virtual view.
class CMatrix;

class CVector3 {
public:
    void Set(float x_, float y_, float z_) {
        x = x_;
        y = y_;
        z = z_;
    }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CColor {
public:
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

struct ParticleVectorView {
    float x;
    float y;
    float z;
};

struct ParticlePropertiesView {
    unsigned char unknown00[112];
    unsigned char fogFlags;
    CColor color71;
    CColor color75;
    unsigned char unknown79;
    CColor color7a;
    unsigned char unknown7e[14];
    int seed;
    ParticleVectorView position;
    ParticleVectorView positionRange;
    ParticleVectorView velocity;
    ParticleVectorView velocityRange;
    ParticleVectorView acceleration;
    float size0cc;
    float size0d0;
    float size0d4;
    float size0d8;
    float size0dc;
};

class CPropertyParticleSystem {
public:
    void SetLocalToWorld(const CMatrix&);
    void SetSeed(int);
    void GetParticleAcceleration(CVector3&) const;
    void GetParticleVelocity(CVector3&, CVector3&) const;
    void GetParticlePosition(CVector3&, CVector3&) const;
    void GetParticleSize(CVector3&, CVector3&, float&) const;
    void GetParticleColor(CColor&, CColor&, CColor&, CColor&) const;
    bool GetFogEnable() const;

    unsigned char unknown000[240];
    ParticlePropertiesView* m_properties;
};

void CPropertyParticleSystem::SetLocalToWorld(const CMatrix&) {
}

void CPropertyParticleSystem::SetSeed(int seed) {
    m_properties->seed = seed;
}

void CPropertyParticleSystem::GetParticleAcceleration(CVector3& acceleration) const {
    acceleration.Set(m_properties->acceleration.x, m_properties->acceleration.y, m_properties->acceleration.z);
}

void CPropertyParticleSystem::GetParticleVelocity(CVector3& velocity, CVector3& range) const {
    velocity.Set(m_properties->velocity.x, m_properties->velocity.y, m_properties->velocity.z);
    range.Set(m_properties->velocityRange.x, m_properties->velocityRange.y, m_properties->velocityRange.z);
}

void CPropertyParticleSystem::GetParticlePosition(CVector3& position, CVector3& range) const {
    position.Set(m_properties->position.x, m_properties->position.y, m_properties->position.z);
    range.Set(m_properties->positionRange.x, m_properties->positionRange.y, m_properties->positionRange.z);
}

void CPropertyParticleSystem::GetParticleSize(CVector3& size, CVector3& range, float& value) const {
    size.Set(m_properties->size0cc, m_properties->size0d4, 0.0f);
    range.Set(m_properties->size0d0, m_properties->size0d8, 0.0f);
    value = m_properties->size0dc;
}
