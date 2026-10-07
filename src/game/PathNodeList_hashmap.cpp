// A fragment of PathNodeList.cpp (0x80055f60): the weak
// dwi::hash_map<unsigned int, CPathNodeList::PathNodes> destructor, which
// frees the table from one entry before the first bucket and clears the
// members (as the inline destructor in publisher_reset.cpp). The template
// is named by the mangled symbols; its members and the entry view are
// inferred (PathNodes is a 12-byte view, from the 16-byte entry). The
// destructor is a weak template copy emitted in this file, defined
// __declspec(weak) out of the class and instantiated explicitly. The rest
// of the file is not part of this unit.
class CPathNodeList {
public:
    struct PathNodes {
        unsigned char data[12];
    };
};

extern "C" void MEM_free(void*);

namespace dwi {
template <class K, class V>
class hash_map {
public:
    struct value_type {
        K first;
        V second;
    };

    ~hash_map();

    value_type* m_data;
    int m_size;
    int m_count;
};

template <class K, class V> __declspec(weak) hash_map<K, V>::~hash_map() {
    MEM_free(m_data - 1);
    m_data = 0;
    m_size = 0;
    m_count = 0;
}
}

template class dwi::hash_map<unsigned int, CPathNodeList::PathNodes>;
