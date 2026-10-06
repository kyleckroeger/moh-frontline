// A fragment of objcreate.cpp (0x8003c1b0): the collision-volume pools.
// Sphere and box volumes share one instance allocator and animated volumes
// have their own; each Allocate function constructs the volume in place in
// the next free element (when there is one) and each Free function returns
// the element. BSGO_Basic::Destroy (weak, empty) follows them. The file name
// is this project's; the original record is objcreate.cpp and
// DelayDestroyObject after these is not reconstructed. The classes and
// globals are named by the mangled symbols; the volume classes are declared
// only as far as their constructors (inline for the sphere and box, which
// store the IVolume and then the derived vtable), and their virtual tables
// are defined elsewhere.
inline void* operator new(unsigned long, void* place) {
    return place;
}

class BSUtilObjectInstanceMemoryAllocator {
public:
    void FreeElement(void*);
    void* GetFreeElement(bool);

    unsigned char unknown00[40];
};

class IVolume {
public:
    IVolume() {}
    virtual ~IVolume();
};

class CVolSphere : public IVolume {
public:
    CVolSphere() {}
    virtual ~CVolSphere();
};

class CVolBox : public IVolume {
public:
    CVolBox() {}
    virtual ~CVolBox();
};

class CAnimatedVolume {
public:
    CAnimatedVolume();
};

class BSGO_Basic {
public:
    void Destroy();
};

extern BSUtilObjectInstanceMemoryAllocator g_pVolumeAllocator;
extern BSUtilObjectInstanceMemoryAllocator g_pAnimatedVolumeAllocator;

void FreeSphereVolume(void* volume) {
    g_pVolumeAllocator.FreeElement(volume);
}

void* AllocateSphereVolume() {
    return new (g_pVolumeAllocator.GetFreeElement(true)) CVolSphere;
}

void FreeBoxVolume(void* volume) {
    g_pVolumeAllocator.FreeElement(volume);
}

void* AllocateBoxVolume() {
    return new (g_pVolumeAllocator.GetFreeElement(true)) CVolBox;
}

void FreeAnimatedVolume(void* volume) {
    g_pAnimatedVolumeAllocator.FreeElement(volume);
}

void* AllocateAnimatedVolume() {
    return new (g_pAnimatedVolumeAllocator.GetFreeElement(true)) CAnimatedVolume;
}

__declspec(weak) void BSGO_Basic::Destroy() {
}
