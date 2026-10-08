// A fragment of particlesystem.cpp (0x8007d394): the weak
// CParticleSystemManager destructor. The manager is a render bin and a scene
// node: CUcodeRenderBin first (its table pointer at +28, after 28 bytes of
// members) and ISceneNode at +32 (the "@32@" thunks of particlesystem_weak.cpp
// adjust by 32). The destructor sets both table pointers, destroys the
// ISceneNode part (inline destructor, then IObserver's) and then the inline
// CUcodeRenderBin and CRenderBin destructors, and frees the object when asked.
// The file name is this project's; the original record is particlesystem.cpp.
// The classes and functions are named by the mangled symbols; only the
// virtuals the destructor needs are declared. CParticleSystemManager declares
// Init (its own virtual, defined elsewhere) first so its global virtual table
// is not emitted here. The scene-node bases' destructors are inline and
// their tables weak in the original, so the compiler's copies are weak
// duplicates, linked to the original copies.
class IDestructible {
public:
    IDestructible() {}
    virtual void MarkForDestruction(int);
    virtual ~IDestructible();
};

class ISubject : public IDestructible {
public:
    ISubject() {}
    virtual ~ISubject();
};

class IObserver : public ISubject {
public:
    IObserver() {}
    virtual ~IObserver();
};

class ISceneNode : public IObserver {
public:
    ISceneNode() : data04(0), data08(0), data0c(0), data10(0), data14(0), data18(0), data1c(0), data20(0) {}
    virtual ~ISceneNode() {}

    int data04;
    int data08;
    int data0c;
    int data10;
    int data14;
    int data18;
    int data1c;
    int data20;
};

class CDmaTag;
class CDmaPacket;

/* CRenderBin's virtuals in table order (the secondary table follows the five
   primary slots). All are weak (inline) in the original; Init's and IsUsed's
   bodies are not part of this view, so they are declared without one, which
   also keeps CRenderBin's table out of this unit. */
class CRenderBin {
    unsigned char unknown00[28]; /* members before the table pointer */

public:
    virtual ~CRenderBin() {}
    virtual int Render(CDmaPacket&, void*) { return 0; }
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&) {}
    virtual bool IsUsed();
};

class CUcodeRenderBin : public CRenderBin {
public:
    virtual ~CUcodeRenderBin() {}
};

class CParticleSystemManager : public CUcodeRenderBin, public ISceneNode {
public:
    virtual void Init();
    virtual ~CParticleSystemManager();
};

__declspec(weak) CParticleSystemManager::~CParticleSystemManager() {
}
