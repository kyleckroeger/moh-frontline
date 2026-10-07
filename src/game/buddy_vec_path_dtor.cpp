// A fragment of buddy.cpp (0x80058f24): the weak dwi::vector<BuddyPathInfo>
// destructor, which destroys each element (iterating with temporary
// iterators from the out-of-line begin and end), clears the size and frees
// the storage when the vector owns it. The containers and BuddyPathInfo are
// named by the mangled symbols; the members, the inline iterator operators
// and the inline clear (its name is inferred) are inferred, and
// BuddyPathInfo is a 4-byte view. The destructor is a weak template copy
// emitted in this file, defined __declspec(weak) out of the class and
// instantiated explicitly. The rest of the file is not part of this unit.
extern "C" void MEM_free(void*);

struct BuddyPathInfo {
    unsigned char data[4];
};

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
    ~vector();
    void clear() {
        for (vec_iter<T> it = begin(); it != end(); ++it)
            it.m_p->~T();
        m_size = 0;
    }
    vec_iter<T> begin();
    vec_iter<T> end();

    int m_size;
    int m_capacity;
    T* m_data;
    bool m_owned;
};

template <class T> __declspec(weak) vector<T>::~vector() {
    clear();
    if (m_owned)
        MEM_free(m_data);
}
}

template class dwi::vector<BuddyPathInfo>;
