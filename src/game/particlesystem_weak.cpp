// A fragment of particlesystem.cpp (0x8008128c): weak copies of header inlines
// emitted in this file: CParticleSystem::IsEmitting (a flag bit at +136) and
// the empty default setters (render type, texture, emission rate and delay,
// lifetimes, particle colour, size, position, velocity and acceleration, fog
// and system acceleration). CParticleSystem, ShapeFile, CColor, CVector3 and
// ERenderType are named by the mangled symbols; CParticleSystem is a
// non-virtual view and its flag bit-field is inferred. The rest of the file
// is not part of this unit.
struct ShapeFile;

struct CColor {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

class CVector3 {
public:
    float x;
    float y;
    float z;
} __attribute__((aligned(8)));

class CParticleSystem {
public:
    enum ERenderType {};

    bool IsEmitting() const;
    void SetRenderType(ERenderType);
    void SetTexture(ShapeFile*);
    void SetEmmisionRate(float);
    void SetEmmisionDelay(float);
    void SetSystemLifetime(float);
    void SetParticleLifetime(float);
    void SetParticleColor(CColor, CColor);
    void SetParticleSize(CVector3, CVector3, float);
    void SetParticlePosition(CVector3, CVector3);
    void SetParticleVelocity(CVector3, CVector3);
    void SetParticleAcceleration(CVector3);
    void SetFogEnable(bool);
    void SetSystemAcceleration(CVector3);

    unsigned char unknown000[136];
    unsigned char m_active : 1;
    unsigned char m_emitting : 1;
    unsigned char m_boundingBoxValid : 1;
    unsigned char m_moving : 1;
    unsigned char m_hasRotation : 1;
    unsigned char m_flags : 3;
};

__declspec(weak) bool CParticleSystem::IsEmitting() const {
    return m_emitting;
}

__declspec(weak) void CParticleSystem::SetRenderType(ERenderType) {
}

__declspec(weak) void CParticleSystem::SetTexture(ShapeFile*) {
}

__declspec(weak) void CParticleSystem::SetEmmisionRate(float) {
}

__declspec(weak) void CParticleSystem::SetEmmisionDelay(float) {
}

__declspec(weak) void CParticleSystem::SetSystemLifetime(float) {
}

__declspec(weak) void CParticleSystem::SetParticleLifetime(float) {
}

__declspec(weak) void CParticleSystem::SetParticleColor(CColor, CColor) {
}

__declspec(weak) void CParticleSystem::SetParticleSize(CVector3, CVector3, float) {
}

__declspec(weak) void CParticleSystem::SetParticlePosition(CVector3, CVector3) {
}

__declspec(weak) void CParticleSystem::SetParticleVelocity(CVector3, CVector3) {
}

__declspec(weak) void CParticleSystem::SetParticleAcceleration(CVector3) {
}

__declspec(weak) void CParticleSystem::SetFogEnable(bool) {
}

__declspec(weak) void CParticleSystem::SetSystemAcceleration(CVector3) {
}
