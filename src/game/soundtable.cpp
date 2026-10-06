// CSoundTable: the loaded sound-event table (closed through the file layer on
// destruction) and the global singleton. CSoundTable and its nested types are
// named by the mangled symbols; members are inferred and are not original.
void TLT_CloseFile(void*);

template <class T> __declspec(weak) void offsetPtr(T*& pointer, int base) {
    if (pointer)
        pointer = reinterpret_cast<T*>(reinterpret_cast<int>(pointer) + base);
}

class CSoundTable {
public:
    struct SKey;
    struct SSoundParms;
    struct SAnimEntry;

    CSoundTable() : m_file(0) {}
    ~CSoundTable();

    void* m_file;
    void* m_data;
};

// The event lookups, Init and EndianSwap come first in the original file and
// are not part of this unit. Init uses the offsetPtr instantiations below,
// which are weak in the image; without Init they are instantiated explicitly
// here, with the template marked __declspec(weak) to keep that binding.

CSoundTable::~CSoundTable() {
    TLT_CloseFile(m_file);
}

template void offsetPtr<CSoundTable::SAnimEntry>(CSoundTable::SAnimEntry*&, int);
template void offsetPtr<CSoundTable::SSoundParms>(CSoundTable::SSoundParms*&, int);
template void offsetPtr<CSoundTable::SKey>(CSoundTable::SKey*&, int);

CSoundTable g_soundMapSingleton;
