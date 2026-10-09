// The end of light.cpp (0x80078980): the static initialisation of the
// animated-light manager (an ISceneNode: base tables through the inline
// constructors, scene-node members cleared, then the manager's own members
// cleared; destructor registered). The rest of the file is in other units or
// not reconstructed. CAnimLightManager and g_AnimLightManager are named by the
// symbols; the object size comes from the symbol, the members are inferred.
// CAnimLightManager declares Destroy (defined elsewhere) first so its table
// stays elsewhere; the weak destructor and ISceneNode's inline destructor and
// table are emitted elsewhere or are weak duplicates.
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

class CAnimLightManager : public ISceneNode {
public:
    virtual void Destroy();
    CAnimLightManager()
        : data30(0), data34(0), data38(0), data3c(0), data40(0), data44(0), data48(0), data4c(0), data50(0),
          data54(0) {}
    virtual ~CAnimLightManager();

    int unknown24[3];
    int data30;
    int data34;
    int data38;
    int data3c;
    int data40;
    int data44;
    int data48;
    int data4c;
    int data50;
    int data54;
};

static CAnimLightManager g_AnimLightManager;
