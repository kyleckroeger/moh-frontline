// A fragment of particlesystem.cpp (0x8007ea84): CParticleSystem::IsActive.
// CParticleSystem is named by the mangled symbols; it is a non-virtual view
// and its flag bits at +136 are inferred (named after the accessors). The
// function is a weak copy of a header inline emitted in this file, so it is
// defined __declspec(weak). The rest of the file is not part of this unit.
class CParticleSystem {
public:
    bool HasRotation() const;
    bool IsActive() const;
    void ValidateBoundingBox(bool);
    bool IsMoving() const;
    bool IsBoundingBoxValid() const;

    unsigned char unknown000[136];
    unsigned char m_active : 1;
    unsigned char m_emitting : 1;
    unsigned char m_boundingBoxValid : 1;
    unsigned char m_moving : 1;
    unsigned char m_hasRotation : 1;
    unsigned char m_flags : 3;
};

__declspec(weak) bool CParticleSystem::IsActive() const {
    return m_active;
}
