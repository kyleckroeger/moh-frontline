// A fragment of scene.cpp (0x800db414): the dwi::vec_const_iter<CCollision> destructor clears the iterator.
// The dwi containers and CCollision are named by the mangled symbols; the
// vector's members (size, capacity, data) and the iterator's pointer are
// inferred, and CCollision is a 32-byte view (the stride the code shows).
// The function is a weak template member emitted in this file, defined
// __declspec(weak) and instantiated explicitly. The rest of the file is not
// part of this unit.
class CCollision {
public:
    unsigned char data[32];
};

namespace dwi {
template <class T>
class vec_const_iter {
public:
    vec_const_iter(const T* p) : m_p(p) {}
    ~vec_const_iter();

    const T* m_p;
};

template <class T>
class vector {
public:

    int m_size;
    int m_capacity;
    T* m_data;
};

template <class T> __declspec(weak) vec_const_iter<T>::~vec_const_iter() {
    m_p = 0;
}
}

template class dwi::vec_const_iter<CCollision>;
