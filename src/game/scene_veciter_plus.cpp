// A fragment of scene.cpp (0x800d8b24): dwi::vec_iter<CCollision>::operator+ returns an iterator advanced by n elements.
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
class vec_iter {
public:
    vec_iter(T* p) : m_p(p) {}
    ~vec_iter();
    vec_iter operator+(int) const;

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

template <class T> __declspec(weak) vec_iter<T> vec_iter<T>::operator+(int n) const {
    return vec_iter<T>(m_p + n);
}
}

template class dwi::vec_iter<CCollision>;
