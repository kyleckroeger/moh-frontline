// A fragment of buddy.cpp (0x80056b6c): dwi::vector<BuddyInfo>::end and
// begin (iterators past the last and to the first element).
// The dwi containers and BuddyInfo are named by the mangled symbols; the
// vector's members (size, capacity, data) and the iterator's pointer are
// inferred, and BuddyInfo is a 32-byte view (the stride the code shows).
// The functions are weak template members emitted in this file, defined
// __declspec(weak) and instantiated explicitly. The rest of the file is not
// part of this unit.
struct BuddyInfo {
    unsigned char data[20];
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
    vec_iter<T> begin();
    vec_iter<T> end();

    int m_size;
    int m_capacity;
    T* m_data;
};

template <class T> __declspec(weak) vec_iter<T> vector<T>::begin() {
    return vec_iter<T>(m_data);
}

template <class T> __declspec(weak) vec_iter<T> vector<T>::end() {
    return vec_iter<T>(m_data + m_size);
}

}

template class dwi::vector<BuddyInfo>;
