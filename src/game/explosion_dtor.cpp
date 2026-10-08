// A fragment of explosion.cpp (0x800cf634): the CExplosion destructor (its virtual table
// pointer, the sphere volume at +16 destroyed, then the inline
// dwi::IVisitor<ISceneNode> destructor, and the object freed when asked). The
// file name is this project's; the original record is explosion.cpp. The classes,
// the template and the functions are named by the mangled symbols; the
// visitor's table lists its destructor and a pure Visit, the members before
// the sphere are not known, and only the virtuals the destructor needs are
// declared. CExplosion declares its Visit override (defined elsewhere) first so its
// global virtual table is not emitted here. The visitor's destructor is
// inline and its table weak in the original, so the compiler's copies are
// weak duplicates, linked to the original copies.
class ISceneNode;

namespace dwi {
template <class T>
class IVisitor {
public:
    virtual ~IVisitor() {}
    virtual void Visit(T&) = 0;
};
}

class IVolume {
public:
    virtual ~IVolume();
};

class CVolSphere : public IVolume {
public:
    virtual ~CVolSphere();

    unsigned char unknown04[28];
} __attribute__((aligned(8)));

class CExplosion : public dwi::IVisitor<ISceneNode> {
public:
    virtual void Visit(ISceneNode&);
    virtual ~CExplosion();

    unsigned char unknown04[12];
    CVolSphere m_sphere;
};

CExplosion::~CExplosion() {
}
