// A fragment of explosion.cpp (0x800cf634): the CExplosion destructor (its virtual table
// pointer, the sphere volume at +16 destroyed, then the inline
// dwi::IVisitor<ISceneNode> destructor, and the object freed when asked) and
// the constructor (the sphere built at the position with the radius through
// an inferred inline constructor; strength, a second value, the bullet type,
// the source node and a flag kept; a noise made at the position (type 52,
// six times the strength); every scene node in the sphere visited; the
// scene's world object told about the explosion when there is one; the 6.0
// constant is an item of the file's .sdata2 pool). The
// file name is this project's; the original record is explosion.cpp. The classes,
// the template and the functions are named by the mangled symbols (the
// argument names and the member names are inferred); the
// visitor's table lists its destructor and a pure Visit, the members before
// the sphere are not known, and only the virtuals the destructor needs are
// declared. CExplosion declares its Visit override (defined elsewhere) first so its
// global virtual table is not emitted here. The visitor's and IVolume's
// destructors are inline and their tables weak in the original, so the compiler's copies are
// weak duplicates, linked to the original copies.
class ISceneNode {
public:
    enum EVolumeType {};
};
class CAIFilterObject;
class CWorldObject;
class CExplosion;
enum EScriptBulletType {};

/* inferred: a 16-byte vector copied as two doubles */
class CVector3 {
public:
    double pair[2];
} __attribute__((aligned(8)));

void MakeNoise(void*, CAIFilterObject*, const CVector3&, int, float, bool, int);

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
    IVolume() {}
    virtual ~IVolume() {}
};

class CVolSphere : public IVolume {
public:
    CVolSphere(CVector3 center, float radius) : m_radius(radius), m_center(center) {}
    virtual IVolume* Create() const;
    virtual ~CVolSphere();

    unsigned char unknown04[8];
    float m_radius;
    CVector3 m_center;
};

class CWorldObject {
public:
    void HandleExplosion(const CExplosion&);
};

class CScene {
public:
    void VisitAllNodesInVolume(dwi::IVisitor<ISceneNode>&, const IVolume&, ISceneNode::EVolumeType, bool);

    unsigned char unknown000[24];
    CWorldObject* m_worldObject;
};

class CExplosion : public dwi::IVisitor<ISceneNode> {
public:
    virtual void Visit(ISceneNode&);
    virtual ~CExplosion();
    CExplosion(CScene&, CVector3, float, float, float, EScriptBulletType, ISceneNode*, bool);

    unsigned char unknown04[12];
    CVolSphere m_sphere;
    float data30;
    float data34;
    EScriptBulletType m_type;
    ISceneNode* m_source;
    bool data40;
};

CExplosion::~CExplosion() {
}

CExplosion::CExplosion(CScene& scene, CVector3 position, float radius, float strength, float value34,
                       EScriptBulletType type, ISceneNode* source, bool flag)
    : m_sphere(position, radius), data30(strength), data34(value34), m_type(type), m_source(source),
      data40(flag) {
    MakeNoise(0, 0, position, 52, strength * 6.0, true, 5);
    scene.VisitAllNodesInVolume(*this, m_sphere, (ISceneNode::EVolumeType)0, true);
    if (scene.m_worldObject)
        scene.m_worldObject->HandleExplosion(*this);
}
