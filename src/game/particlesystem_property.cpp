// A fragment of particlesystem.cpp (0x8007f2ac); the file name is this project's. CPropertyParticleSystem accessors in the middle of the file: the
// local-to-world matrix and the lifetimes, emission delay and rate, render
// type and seed read from the particle-property record. The class names come
// from the mangled symbols; the members and the property-record view are
// inferred from offsets, and the class is a non-virtual view. The unit starts
// with the particle rotation and the system acceleration and initial-velocity
// getters (GetParticleAlpha before them is in particlesystem_color.cpp).
//
// CVector3 view: four floats, 8-byte aligned, overlaid with two doubles. The
// union is inferred from the code, not the original declaration: the system
// vectors are copied as two lfd/stfd pairs, which an implicit copy through the
// double pair reproduces.
class CVector3 {
public:
    union {
        struct {
            float x;
            float y;
            float z;
            float w;
        };
        double pair[2];
    };
} __attribute__((aligned(8)));

class CMatrix {
public:
    CMatrix& operator=(const CMatrix&);

    float m[16];
};

struct ParticlePropertiesView {
    unsigned char unknown00[121];
    unsigned char renderType;
    unsigned char unknown7a[8];
    short emissionRate;
    short emissionDelay;
    unsigned char unknown86[2];
    float systemLifetime;
    int seed;
    unsigned char unknown90[80];
    float particleLifetime;
    float rotation;
    float rotationRate;
};

class CPropertyParticleSystem {
public:
    void GetParticleRotation(float&, float&) const;
    void GetSystemAcceleration(CVector3&) const;
    void GetSystemInitialVelocity(CVector3&) const;
    void GetLocalToWorld(CMatrix&) const;
    float GetParticleLifetime() const;
    float GetSystemLifetime() const;
    short GetEmmisionDelay() const;
    short GetEmmisionRate() const;
    unsigned char GetRenderType() const;
    int GetSeed() const;

    unsigned char unknown000[144];
    CMatrix m_localToWorld;
    CVector3 m_initialVelocity;
    CVector3 m_acceleration;
    ParticlePropertiesView* m_properties;
};

void CPropertyParticleSystem::GetParticleRotation(float& rotation, float& rate) const {
    rotation = m_properties->rotation;
    rate = m_properties->rotationRate;
}

void CPropertyParticleSystem::GetSystemAcceleration(CVector3& v) const {
    v = m_acceleration;
}

void CPropertyParticleSystem::GetSystemInitialVelocity(CVector3& v) const {
    v = m_initialVelocity;
}

void CPropertyParticleSystem::GetLocalToWorld(CMatrix& m) const {
    m = m_localToWorld;
}

float CPropertyParticleSystem::GetParticleLifetime() const {
    return m_properties->particleLifetime;
}

float CPropertyParticleSystem::GetSystemLifetime() const {
    return m_properties->systemLifetime;
}

short CPropertyParticleSystem::GetEmmisionDelay() const {
    return m_properties->emissionDelay;
}

short CPropertyParticleSystem::GetEmmisionRate() const {
    return m_properties->emissionRate;
}

unsigned char CPropertyParticleSystem::GetRenderType() const {
    return m_properties->renderType;
}

int CPropertyParticleSystem::GetSeed() const {
    return m_properties->seed;
}
