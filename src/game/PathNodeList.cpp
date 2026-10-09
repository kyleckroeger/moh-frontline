// The static initialisation of PathNodeList.cpp (0x80055f1c): the path-node
// map starts with no table (its weak destructor registered). The rest of the
// file is in other units or not reconstructed. CPathNodeList, PathNodes, the
// hash map and s_pathNodeMap are named by the symbols; the map's members and
// default constructor are inferred (as in PathNodeList_hashmap.cpp).
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

class CPathNodeList {
public:
    struct PathNodes;

    static dwi::hash_map<unsigned int, PathNodes> s_pathNodeMap;
};

dwi::hash_map<unsigned int, CPathNodeList::PathNodes> CPathNodeList::s_pathNodeMap;
