// A fragment of publisher.cpp (0x800d7e08): CPublisher::Shutdown deletes the
// publisher (the hash map's destructor frees the table, whose allocation
// starts one entry before the first bucket, and clears the members) and
// CPublisher::Reset empties the map (every bucket key set to -1, the sentinel
// keys before the first and after the last bucket set to -2, the count
// cleared). CPublisher, its SItem, ISubject and the dwi::hash_map template are
// named by the mangled symbols (hash_map<ISubject*, CPublisher::SItem>); the
// members are inferred views, and the destructor and clear are inline here.
// The rest of the file is not part of this unit.
class ISubject;

extern "C" void MEM_free(void*);

namespace dwi {
template <class K, class V>
class hash_map {
public:
    struct value_type {
        K first;
        V second;
    };

    ~hash_map() {
        MEM_free(m_data - 1);
        m_data = 0;
        m_size = 0;
        m_count = 0;
    }

    void clear() {
        for (value_type* it = m_data; it != m_data + m_size; ++it)
            it->first = (K)-1;
        m_data[-1].first = (K)-2;
        m_data[m_size].first = (K)-2;
        m_count = 0;
    }

    value_type* m_data;
    int m_size;
    int m_count;
};
}

class CPublisher {
public:
    struct SItem {
        unsigned char unknown[12];
    };

    static void Shutdown();
    static void Reset();

    dwi::hash_map<ISubject*, SItem> m_map;

    static CPublisher* sm_pThePublisher;
};

void CPublisher::Shutdown() {
    delete sm_pThePublisher;
}

void CPublisher::Reset() {
    sm_pThePublisher->m_map.clear();
}
