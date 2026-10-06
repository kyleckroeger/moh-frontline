// CPropertyParticleSystem accessors in the middle of the file: the
// local-to-world matrix and the lifetimes, emission delay and rate, render
// type and seed read from the particle-property record. The class names come
// from the mangled symbols; the members and the property-record view are
// inferred from offsets, and the class is a non-virtual view. The system
// acceleration and initial-velocity getters before them copy a 16-byte vector
// as two doublewords, which this view cannot reproduce without inventing a
// type, so the unit starts after them.
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
};

class CPropertyParticleSystem {
public:
    void GetLocalToWorld(CMatrix&) const;
    float GetParticleLifetime() const;
    float GetSystemLifetime() const;
    short GetEmmisionDelay() const;
    short GetEmmisionRate() const;
    unsigned char GetRenderType() const;
    int GetSeed() const;

    unsigned char unknown000[144];
    CMatrix m_localToWorld;
    unsigned char m_initialVelocity[16];
    unsigned char m_acceleration[16];
    ParticlePropertiesView* m_properties;
};

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
