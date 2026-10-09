// The end of AIFilter.cpp (0x8005dd48): the static initialisation of the
// global AI filter object (its inline constructor clears its members and
// stores three successive values of its flag word at +20; the meaning of the
// flags is not known) and of the recent-sounds vector (a fast_vec over the
// file's 16-entry static storage, not owning it), each with its destructor
// registered, then the weak dwi::fast_vec<NoiseInfo> destructor, which frees
// the storage when the vector owns it. The template, CAIFilterGlobal,
// NoiseInfo and the objects are named by the mangled symbols; the members
// are inferred (as in compartment_weak.cpp and AIFilter_astar.cpp), the
// NoiseInfo size comes from the storage symbol, and the constructors are
// inferred inlines. The destructor is a weak template copy emitted in this
// file, defined __declspec(weak) out of the class and instantiated by the
// registration (so it follows the static initialisation). The unit defines the file's .bss block. The rest of the file
// is in other units.
struct NoiseInfo {
    unsigned char data[12];
};

extern "C" void MEM_free(void*);

namespace dwi {
template <class T>
class fast_vec {
public:
    fast_vec(T* data, int capacity) : m_data(data), m_unknown4(0), m_size(0), m_capacity(capacity), m_owned(false), m_growth(0) {}
    ~fast_vec();

    T* m_data;
    int m_unknown4;
    int m_size;
    int m_capacity;
    bool m_owned;
    int m_growth;
};

template <class T> __declspec(weak) fast_vec<T>::~fast_vec() {
    if (m_owned && m_data)
        MEM_free(m_data);
}
}

class CAIFilterGlobal {
public:
    CAIFilterGlobal() {
        m_flags = 0x44b0;
        m_flags = 0x44d0;
        m_unknown00 = 0;
        m_time = 0.0f;
        m_splines = 0;
        m_pathNodes = 0;
        m_flags = 0x4ca0;
    }
    ~CAIFilterGlobal();

    int m_unknown00;
    float m_time;
    unsigned char unknown08[4];
    void* m_splines;
    void* m_pathNodes;
    int m_flags;
};

CAIFilterGlobal g_aigAIFilterGlobalObject;
static NoiseInfo recent_sounds_mem[16];
static dwi::fast_vec<NoiseInfo> recent_sounds(recent_sounds_mem, 16);
