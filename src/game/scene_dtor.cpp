// A fragment of scene.cpp (0x800dc620): the CScene destructor shuts the
// scene down while it still has nodes (the count at +56), then destroys its
// containers: the fast vectors (freeing owned storage), the collision vector
// (destroying each collision, then freeing) and the two extent lists (nodes
// back to the free list, owned node storage freed). CScene, CCollision,
// ISceneNode::SExtent and the dwi containers are named by the mangled symbols;
// the scene's members, the container layouts and their inline destructors and
// clear functions are inferred (as in scene_reset.cpp), and the fast vectors'
// element types are not known (void* here). The collision iterator's weak
// destructor copy that this compiles is discarded as a duplicate of the one in
// scene_veciter_dtor.cpp. The rest of the file is not part of this unit.
extern "C" void MEM_free(void*);

class CCollision {
public:
    ~CCollision();

    unsigned char data[32];
};

class ISceneNode {
public:
    struct SExtent {
        unsigned char data[16];
    };
};

class CPlayerObject;

namespace dwi {
template <class T>
class vec_iter {
public:
    vec_iter(T* p) : m_p(p) {}
    ~vec_iter() { m_p = 0; }
    bool operator!=(const vec_iter& other) const { return m_p != other.m_p; }
    vec_iter& operator++() {
        m_p++;
        return *this;
    }

    T* m_p;
};

template <class T>
class vector {
public:
    ~vector() {
        clear();
        if (m_owned)
            MEM_free(m_data);
    }
    vec_iter<T> begin();
    vec_iter<T> end();
    void clear() {
        for (vec_iter<T> it = begin(); it != end(); ++it)
            it.m_p->~T();
        m_size = 0;
    }

    int m_size;
    int m_capacity;
    T* m_data;
    bool m_owned;
};

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
        clear();
        if (m_owned)
            MEM_free(m_pool);
        m_pool = 0;
    }
    void clear() {
        list_node<T>* node = m_head.m_next;

        while (node != &m_head) {
            list_node<T>* next = node->m_next;
            node->m_prev = 0;
            node->m_next = m_free;
            m_free = node;
            node = next;
        }
        m_head.m_next = &m_head;
        m_head.m_prev = &m_head;
        m_count = 0;
    }

    int m_count;
    int m_unknown4;
    list_node<T>* m_pool;
    list_node<T>* m_free;
    list_node<T> m_head;
    bool m_owned;
};

template <class T>
class fast_vec {
public:
    ~fast_vec() {
        if (m_owned && m_data)
            MEM_free(m_data);
    }
    void clear() { m_size = 0; }

    T* m_data;
    int m_unknown4;
    int m_size;
    int m_capacity;
    bool m_owned;
    int m_growth;
};
}

class CScene {
public:
    ~CScene();
    void Shutdown();

    void* m_node0;
    void* m_node4;
    CPlayerObject* m_players[4];
    void* m_value24[8];
    int m_count56;
    void* m_value60[3];
    unsigned char unknown072[4];
    void* m_value76;
    dwi::fast_vec<void*> m_vec80;
    dwi::fast_vec<void*> m_vec104;
    dwi::list<ISceneNode::SExtent> m_extents128;
    dwi::list<ISceneNode::SExtent> m_extents172;
    dwi::fast_vec<void*> m_vec216;
    dwi::fast_vec<void*> m_vec240;
    dwi::vector<CCollision> m_collisions;
    dwi::fast_vec<void*> m_vec280;
    dwi::fast_vec<void*> m_vec304;
};

CScene::~CScene() {
    if (m_count56 > 0)
        Shutdown();
}
