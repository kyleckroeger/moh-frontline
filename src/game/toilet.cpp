// The static initialisation of toilet.cpp (0x8001e04c): the file's string
// repository (a hash map from names to names) starts with no table, its weak
// destructor registered. The rest of the file is in other units.
// g_Repository and the hash map are named by the symbols; the map's members
// and default constructor are inferred (as in toilet_hashmap.cpp).
namespace dwi {
template <class K, class V>
class hash_map {
public:
    hash_map() : m_data(0), m_size(0), m_count(0) {}
    ~hash_map();

    void* m_data;
    int m_size;
    int m_count;
};
}

static dwi::hash_map<char*, char*> g_Repository;
