// A fragment of compartment.cpp (0x800770a4): the weak
// dwi::fast_vec<CLight*>::reserve (adopts a caller's buffer after freeing
// owned storage) and destructor.
// The classes, templates and functions are named by the mangled symbols; the
// fast_vec members are inferred (the layout differs from the view in
// scene.cpp, which only uses the first three words), CCompartment is a
// non-virtual view, the record types are left incomplete, and explicit
// instantiations reproduce the order of the weak copies. The rest of the file
// is not part of this unit.
extern "C" void MEM_free(void*);

class CLight;

namespace dwi {
template <class T>
class fast_vec {
public:
    fast_vec(int);
    void push_back(T);
    T* begin();
    T* end();
    int size() const;
    void reserve(T*, int);
    ~fast_vec();

    T* m_data;
    int m_unknown4;
    int m_size;
    int m_capacity;
    bool m_owned;
    int m_growth;
};

template <class T> __declspec(weak) void fast_vec<T>::reserve(T* buffer, int capacity) {
    if (m_owned && m_data)
        MEM_free(m_data);
    m_data = buffer;
    m_capacity = capacity;
    m_owned = false;
}

template <class T> __declspec(weak) fast_vec<T>::~fast_vec() {
    if (m_owned && m_data)
        MEM_free(m_data);
}
}

template class dwi::fast_vec<CLight*>;
