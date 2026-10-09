// A fragment of light.cpp (0x80077bd4): CPropertyAnimLight::Destroy (the
// light, itself a node of the animated-light manager's second pool, unlinked
// from the in-use list and returned to the free list; the pool view and its
// Free inline are inferred, as in light_statics.cpp), then the
// CPropertyAnimLight destructor (its
// virtual table pointer, then the CLight destructor inlined (global, later in
// this file; how the original inlined it is not known, so this view declares
// it inline), then the inline IMovingSceneNode and ISceneNode destructors and
// IObserver's, and the object freed when asked). The file name is this
// project's; the original record is light.cpp. The classes and functions are
// named by the mangled symbols; only the virtuals the destructor needs are
// declared. Each class declares its own override (MarkForDestruction or
// Destroy, defined elsewhere; BeginUpdate for CPropertyAnimLight) first so
// the global virtual tables are not emitted here.
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

/* Inferred: a pool node, a light with the next link after it. */
struct AnimLightNodeView {
    unsigned char light[224];
    AnimLightNodeView* next;
    unsigned char unknownE4[12];
};

/* Inferred: a fixed pool of light nodes with in-use and free lists. */
struct AnimLightPoolView {
    void Free(AnimLightNodeView* node) {
        if (node == m_used) {
            m_used = node->next;
        } else {
            for (AnimLightNodeView* n = m_used; n; n = n->next) {
                if (n->next == node) {
                    n->next = node->next;
                    break;
                }
            }
        }
        m_count--;
        node->next = m_free;
        m_free = node;
    }

    AnimLightNodeView* m_data;
    AnimLightNodeView* m_used;
    AnimLightNodeView* m_free;
    int m_capacity;
    int m_count;
};

class CAnimLightManager {
public:
    unsigned char unknown00[48];
    AnimLightPoolView m_pool30;
    AnimLightPoolView m_pool44;
};

extern CAnimLightManager g_AnimLightManager;

class CPropertyAnimLight : public CLight {
public:
    virtual void BeginUpdate(float);
    virtual void Destroy();
    virtual ~CPropertyAnimLight();
};

void CPropertyAnimLight::Destroy() {
    g_AnimLightManager.m_pool44.Free((AnimLightNodeView*)this);
}

CPropertyAnimLight::~CPropertyAnimLight() {
}
