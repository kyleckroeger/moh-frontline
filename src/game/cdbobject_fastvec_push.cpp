// A fragment of cdbobject.cpp (0x80070c5c): the weak
// dwi::fast_vec<SLocalFrame>::push_back (the frame is copied into the next
// element: the node, then the line through its assignment). The template,
// SLocalFrame and CLine3 are named by the mangled symbols; the members are
// inferred views. The function is a weak template copy, defined
// __declspec(weak) out of the class and instantiated explicitly; the
// vector's destructor (cdbobject_fastvec_frame.cpp) is not defined here.
extern "C" void MEM_free(void*);

/* Inferred: CVector3 as four floats overlaid with two doubles. */
union CVector3Data {
    double pair[2];
    float v[4];
};

class CVector3 {
public:
    CVector3Data d;
} __attribute__((aligned(8)));

/* The line view of capsule_create.cpp (16-aligned): the stored points and
   direction, then cached values and their flags; its assignment copies the
   members unless assigned to itself, and its destructor is inline (weak in
   the original). */
class CLine3 {
public:
    CLine3() {}
    ~CLine3() {}
    CLine3& operator=(const CLine3& other) {
        if (&other != this) {
            m_start = other.m_start;
            m_end = other.m_end;
            m_dir = other.m_dir;
            m_value30 = other.m_value30;
            m_value34 = other.m_value34;
            m_flag38 = other.m_flag38;
            m_flag39 = other.m_flag39;
        }
        return *this;
    }

    CVector3 m_start;
    CVector3 m_end;
    CVector3 m_dir;
    float m_value30;
    float m_value34;
    unsigned char m_flag38;
    unsigned char m_flag39;
} __attribute__((aligned(16)));

struct CDBBranchNode;

/* Inferred: a branch node and the part of the line inside it (80 bytes). */
struct SLocalFrame {
    SLocalFrame();
    ~SLocalFrame();

    const CDBBranchNode* m_node;
    CLine3 m_line;
};

namespace dwi {
// Inferred: the vector over caller storage (as in cdbobject_fastvec_frame.cpp).
template <class T>
class fast_vec {
public:
    void push_back(T);
    ~fast_vec();

    T* m_data;
    int m_unknown4;
    int m_size;
    int m_capacity;
    bool m_owned;
    int m_growth;
};

template <class T> __declspec(weak) void fast_vec<T>::push_back(T x) {
    m_data[m_size++] = x;
}
}

template class dwi::fast_vec<SLocalFrame>;
