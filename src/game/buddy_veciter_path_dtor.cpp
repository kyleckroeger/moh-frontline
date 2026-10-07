// A fragment of buddy.cpp (0x80058520): the dwi::vec_iter<BuddyPathInfo> destructor clears the iterator.
// The dwi containers and BuddyPathInfo are named by the mangled symbols; the
// vector's members (size, capacity, data) and the iterator's pointer are
// inferred, and BuddyPathInfo is a 32-byte view (the stride the code shows).
// The function is a weak template member emitted in this file, defined
// __declspec(weak) and instantiated explicitly. The rest of the file is not
// part of this unit.
struct BuddyPathInfo {
    unsigned char data[4];
};

namespace dwi {
template <class T>
class vec_iter {
public:
    vec_iter(T* p) : m_p(p) {}
    ~vec_iter();

    T* m_p;
};

template <class T>
class vector {
public:
    vec_iter<T> end();
    vec_iter<T> begin();

    int m_size;
    int m_capacity;
    T* m_data;
};

template <class T> __declspec(weak) vec_iter<T>::~vec_iter() {
    m_p = 0;
}
}

template class dwi::vec_iter<BuddyPathInfo>;
