// A fragment of bsbifunc.cpp (0x80029370): script built-ins that deactivate a
// particle system and set a particle system's initial velocity (from three
// floats), followed by the weak, empty CParticleSystem::SetSystemInitialVelocity.
// Each reads its arguments below the script stack top (the system first) and
// pops the built-in's arguments; a null system is skipped. The file name is
// this project's; the original record is bsbifunc.cpp and the built-ins around
// these are not reconstructed. The functions and classes are named by the
// mangled symbols; CRenderBin and CParticleSystem are declared with their
// virtual functions in the order of __vt__15CParticleSystem (the pointer at
// +28 after 28 bytes of members; the destructors declared and defined
// elsewhere), the 18 null entries at +60 to +128 as pure virtual functions whose
// names are unknown; return types not visible in the code are left as int or
// void, and the built-in record view is inferred.
//
// CVector3 view: four floats, 8-byte aligned, built by an inline constructor
// from x, y and z (inferred; the copy to the by-value argument is two lfd/stfd
// pairs).
class CVector3 {
public:
    CVector3(float ax, float ay, float az) : x(ax), y(ay), z(az) {}

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CDmaPacket;
class CDmaTag;
class CMatrix;
class CColor;
class ShapeFile;
class CDrawContext;

class CRenderBin {
    unsigned char unknown00[28];

public:
    virtual ~CRenderBin();
    virtual void Render(CDmaPacket&, void*);
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual int IsUsed();
};

class CParticleSystem : public CRenderBin {
public:
    enum ERenderType {};

    virtual ~CParticleSystem();
    virtual void DeActivate();
    virtual void Terminate();
    virtual int IsActive() const;
    virtual int IsEmitting() const;
    virtual int IsBoundingBoxValid() const;
    virtual int IsMoving() const;
    virtual int HasRotation() const;
    virtual int GetTexture() const;
    virtual void unknown3c() = 0;
    virtual void unknown40() = 0;
    virtual void unknown44() = 0;
    virtual void unknown48() = 0;
    virtual void unknown4c() = 0;
    virtual void unknown50() = 0;
    virtual void unknown54() = 0;
    virtual void unknown58() = 0;
    virtual void unknown5c() = 0;
    virtual void unknown60() = 0;
    virtual void unknown64() = 0;
    virtual void unknown68() = 0;
    virtual void unknown6c() = 0;
    virtual void unknown70() = 0;
    virtual void unknown74() = 0;
    virtual void unknown78() = 0;
    virtual void unknown7c() = 0;
    virtual void unknown80() = 0;
    virtual void SetRenderType(ERenderType);
    virtual void SetTexture(ShapeFile*);
    virtual void SetLocalToWorld(const CMatrix&);
    virtual void SetEmmisionRate(float);
    virtual void SetEmmisionDelay(float);
    virtual void SetSystemLifetime(float);
    virtual void SetParticleLifetime(float);
    virtual void SetSystemInitialVelocity(CVector3);
};

struct BSBuiltinView {
    void (*function)(int**, void*);
    unsigned char unknown04[6];
    short argumentCount;
    short parameterCount;
    unsigned char unknown0e[2];
};

extern BSBuiltinView* g_pBuiltInFunctions;
extern int g_iCurrentBIFIndex;

void BIFunc_DestroyParticleSystem(int** stack, void*) {
    CParticleSystem* system = *(CParticleSystem**)(*stack - (g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount - 1));
    if (system)
        system->DeActivate();
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

void BIFunc_SetParticleSystemVelocity(int** stack, void*) {
    int count = g_pBuiltInFunctions[g_iCurrentBIFIndex].parameterCount;
    CParticleSystem* system = *(CParticleSystem**)(*stack - (count - 1));
    float x = *(float*)(*stack - (count - 2));
    float y = *(float*)(*stack - (count - 3));
    float z = *(float*)(*stack - (count - 4));
    if (system) {
        CVector3 velocity(x, y, z);
        system->SetSystemInitialVelocity(velocity);
    }
    *stack -= g_pBuiltInFunctions[g_iCurrentBIFIndex].argumentCount;
}

__declspec(weak) void CParticleSystem::SetSystemInitialVelocity(CVector3) {
}
