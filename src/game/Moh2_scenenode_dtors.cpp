// A fragment of Moh2.cpp (0x8001ad30): the weak IMovingSceneNode destructor
// (its table pointer, then the inline ISceneNode destructor and IObserver's;
// the object freed when asked), followed by the weak ISceneNode destructor
// itself, emitted after its first caller. The file name is this project's;
// the original record is Moh2.cpp. The classes and destructors are named by
// the mangled symbols; the members are not part of this view, and only the
// virtuals the destructors need are declared. Defining IMovingSceneNode's
// weak destructor out of line makes it the key function, so the compiler
// emits a global copy of the weak original table; it and the compiler's copy
// of ISceneNode's weak table are weak duplicates, linked to the originals.
class IDestructible {
public:
    virtual void MarkForDestruction(int);
    virtual ~IDestructible();
};

class ISubject : public IDestructible {
public:
    virtual ~ISubject();
};

class IObserver : public ISubject {
public:
    virtual ~IObserver();
};

class ISceneNode : public IObserver {
public:
    virtual ~ISceneNode() {}
};

class IMovingSceneNode : public ISceneNode {
public:
    virtual ~IMovingSceneNode();
};

__declspec(weak) IMovingSceneNode::~IMovingSceneNode() {
}
