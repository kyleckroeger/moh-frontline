// A fragment of world_object.cpp (0x800b7608): the CWorldObject destructor
// (its virtual table pointer; then the members in reverse order: the world
// volume at +272, the request list at +200 and the compartment list at +64
// (each returns its nodes to the free list, destroying the compartments,
// resets its sentinel and count and frees owned node storage; the
// compartment list's sentinel element is destroyed too) and the memory
// factory at +36 (frees owned storage, clears its words); then the inline
// ISceneNode destructor and IObserver's; the object freed when asked). The
// file name is this project's; the original record is world_object.cpp. The
// classes, templates and functions are named by the mangled symbols
// (dwi::list<CCompartment>, dwi::list<CWorldObject::SRequest>,
// dwi::CMemFactory, whose destructors are inline here); the list view is as
// in world_volume_list.cpp, the factory's members are inferred from its weak
// destructor, the request's 40 bytes from the list layout, and the other
// members and names are inferred. Only the virtuals the destructor needs are
// declared. CWorldObject declares Draw (defined elsewhere; the result type is
// not known) first so its global virtual table is not emitted here.
// ISceneNode's destructor is inline and its table weak in the original, so
// the compiler's copies are weak duplicates, linked to the original copies.
extern "C" void MEM_free(void*);
class CDrawContext;

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

namespace dwi {
template <class T>
struct list_node {
    list_node* m_prev;
    list_node* m_next;
    T m_data;
};

template <class T>
class list {
public:
    ~list() {
        list_node<T>* node = m_head.m_next;
        list_node<T>* next;

        while (node != &m_head) {
            next = node->m_next;
            node->~list_node<T>();
            node->m_prev = 0;
            node->m_next = m_free;
            m_free = node;
            node = next;
        }
        m_head.m_next = &m_head;
        m_head.m_prev = &m_head;
        m_count = 0;
        if (m_owned)
            MEM_free(m_pool);
        m_pool = 0;
    }

    int m_count;
    int m_unknown4;
    list_node<T>* m_pool;
    list_node<T>* m_free;
    list_node<T> m_head;
    bool m_owned;
};

class CMemFactory {
public:
    ~CMemFactory() {
        if (m_owned)
            MEM_free(m_data);
        m_data = 0;
        m_unknown4 = 0;
        m_unknown8 = 0;
        m_unknownc = 0;
        m_unknown10 = 0;
    }

    void* m_data;
    int m_unknown4;
    int m_unknown8;
    int m_unknownc;
    int m_unknown10;
    bool m_owned;
};
}

class CCompartment {
public:
    ~CCompartment();

    unsigned char unknown00[104];
} __attribute__((aligned(8)));

class CWorldVolume {
public:
    virtual ~CWorldVolume();

    unsigned char unknown04[76];
};

class CWorldObject : public ISceneNode {
public:
    struct SRequest {
        unsigned char unknown00[40];
    };

    virtual void Draw(CDrawContext&); /* result type not known */
    virtual ~CWorldObject();

    dwi::CMemFactory m_factory;
    unsigned char unknown3c[4];
    dwi::list<CCompartment> m_compartments;
    dwi::list<SRequest> m_requests;
    unsigned char unknown10c[4];
    CWorldVolume m_volume;
};

CWorldObject::~CWorldObject() {
}
