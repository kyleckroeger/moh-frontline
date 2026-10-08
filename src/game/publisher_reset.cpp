// A fragment of publisher.cpp (0x800d7e08): CPublisher::Shutdown deletes the
// publisher (the hash map's destructor frees the table, whose allocation
// starts one entry before the first bucket, and clears the members) and
// CPublisher::Reset empties the map (every bucket key set to -1, the sentinel
// keys before the first and after the last bucket set to -2, the count
// cleared). CPublisher::Init creates the publisher (its map sized to the first
// prime above the count from the header's prime list, allocated with a spare
// entry on each side, then cleared). CPublisher, its SItem, ISubject,
// IObserver and the dwi::hash_map template are named by the mangled symbols
// (hash_map<ISubject*, CPublisher::SItem>); the members are inferred views,
// and the map's constructor, destructor, clear and NextPrime are inline (the
// name of NextPrime is inferred). The compiler also emits an unreferenced
// weak copy of the map's destructor, which the original does not contain; it
// is discarded. The rest of the file is not part of this unit.
class ISubject;
class IObserver;
enum ESubjectEvent {};

extern "C" void MEM_free(void*);
void* DWI_allocalign(const char*, int, int, int);

// Header-defined hash-table primes; every unit that includes the header carries a copy.
namespace dwi {
static const unsigned long dwi_prime_list[27] = {
    53,        97,        193,       389,       769,       1543,      3079,      6151,      12289,
    24593,     49157,     98317,     196613,    393241,    786433,    1572869,   3145739,   6291469,
    12582917,  25165843,  50331653,  100663319, 201326611, 402653189, 805306457, 1610612741, 3221225473u,
};
}

namespace dwi {
template <class K, class V>
class hash_map {
public:
    struct value_type {
        K first;
        V second;
    };

    hash_map(int count) {
        int size = NextPrime(count + 1);
        m_data = 0;
        m_size = 0;
        m_count = 0;
        m_size = size;
        m_data = (value_type*)DWI_allocalign(0, (size + 2) * sizeof(value_type), 16, 1024) + 1;
        clear();
    }

    static int NextPrime(int n) {
        const unsigned long* p = dwi_prime_list;
        const unsigned long* end = dwi_prime_list + 27;
        for (; p != end; p++) {
            if ((int)*p >= n)
                return *p;
        }
        return *(end - 1);
    }

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

typedef void (*SubjectCallback)(IObserver&, ISubject**, ESubjectEvent);

class CPublisher {
public:
    struct SItem {
        IObserver* m_observer;
        ISubject** m_slot;
        SubjectCallback m_callback;
    };

    CPublisher(int count) : m_map(count) {}

    static void Init(int);
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

void CPublisher::Init(int count) {
    sm_pThePublisher = new CPublisher(count);
}
