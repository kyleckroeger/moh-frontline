// A fragment of light.cpp (0x80077720): the weak CInstancedAnimLight
// destructor (its virtual table pointer, then the CPropertyAnimLight and
// CLight destructors inlined (both global, later in this file; how the
// original inlined them is not known, so this view declares them inline), then
// the inline IMovingSceneNode and ISceneNode destructors and IObserver's, and
// the object freed when asked). The file name is this project's; the original
// record is light.cpp. The classes and functions are named by the mangled
// symbols; only the virtuals the destructor needs are declared. Each class
// declares its own override (MarkForDestruction or Destroy, defined elsewhere)
// first so the global virtual tables are not emitted here.
// IMovingSceneNode's and ISceneNode's destructors are inline and their
// tables weak in the original, so the compiler's copies are weak duplicates,
// linked to the original copies.
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

class IMovingSceneNode : public ISceneNode {
public:
    IMovingSceneNode() {}
    virtual ~IMovingSceneNode() {}
};

class CLight : public IMovingSceneNode {
public:
    virtual void MarkForDestruction(int);
    virtual ~CLight() {}
};

class CPropertyAnimLight : public CLight {
public:
    virtual void Destroy();
    virtual ~CPropertyAnimLight() {}
};

class CInstancedAnimLight : public CPropertyAnimLight {
public:
    virtual void Destroy();
    virtual ~CInstancedAnimLight();
};

__declspec(weak) CInstancedAnimLight::~CInstancedAnimLight() {
}
