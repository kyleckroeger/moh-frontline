// A fragment of world_object.cpp (0x800b80d8): the weak
// dwi::list<CWorldObject::SRequest> and dwi::list<CCompartment> destructors,
// which return every node to the free list (destroying its element), reset
// the sentinel and the count, free the node storage when the list owns it,
// and then destroy the sentinel node. The weak dwi::list_node<CCompartment>
// destructor after them is not part of this unit (its out-of-line copy is
// not emitted from this view). The template is named by the mangled
// symbols; its members, the node layout (links before the element, the
// sentinel node held in the list) and the element view (40 bytes, from the
// offset of the ownership flag; CCompartment's from the sentinel's size,
// with its destructor defined elsewhere) are inferred. The destructors are weak
// template copies emitted in this file, defined __declspec(weak) out of the
// class and instantiated explicitly. The rest of the file is not part of
// this unit.
class CWorldObject {
public:
    struct SRequest {
        unsigned char data[40];
    };
};

class CCompartment {
public:
    ~CCompartment();

    unsigned char data[104];
};

extern "C" void MEM_free(void*);

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
    ~list();

    int m_count;
    int m_unknown4;
    list_node<T>* m_pool;
    list_node<T>* m_free;
    list_node<T> m_head;
    bool m_owned;
};

template <class T> __declspec(weak) list<T>::~list() {
    list_node<T>* next;
    list_node<T>* node = m_head.m_next;

    while (node != &m_head) {
        next = node->m_next;
        if (node)
            node->m_data.~T();
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
}

template class dwi::list<CWorldObject::SRequest>;
template class dwi::list<CCompartment>;
