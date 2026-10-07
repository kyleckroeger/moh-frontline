// CSoundTable: Init (loads the event and animation tables named by the level's
// resource 12, converts their byte order and relocates their record pointers),
// EndianSwap, the destructor (closes the table through the file layer) and the
// global singleton. The byte-order helpers are the inlined ones described in
// propdat.cpp (inferred). CSoundTable and its nested types are named by the
// mangled symbols; members and record layouts are inferred and are not
// original.
void TLT_CloseFile(void*);

enum TLTResourceID {};
struct LevelFileContentsStruct_;

// Inferred view of the level resource entry.
struct LevelResourceView {
    unsigned char unknown00[8];
    struct {
        unsigned char unknown00[92];
        const char* m_eventFile;
        unsigned char unknown60[4];
        const char* m_animFile;
    }* m_names;
};

LevelResourceView* TLT_FindNextResourceByType(TLTResourceID, LevelFileContentsStruct_*);
void* TLT_LoadFileFromLevelBigFile(const char*, int*);

inline void ChangeEndian(short& value) {
    unsigned char bytes[2];
    *reinterpret_cast<short*>(bytes) = value;
    unsigned char c = bytes[0];
    bytes[0] = bytes[1];
    bytes[1] = c;
    value = *reinterpret_cast<short*>(bytes);
}

inline void ChangeEndian(int& value) {
    unsigned char bytes[4];
    *reinterpret_cast<int*>(bytes) = value;
    unsigned char c = bytes[0];
    bytes[0] = bytes[3];
    bytes[3] = c;
    c = bytes[1];
    bytes[1] = bytes[2];
    bytes[2] = c;
    value = *reinterpret_cast<int*>(bytes);
}

template <class T> inline void ChangeEndian(T& value) {
    ChangeEndian(*reinterpret_cast<int*>(&value));
}


template <class T> __declspec(weak) void offsetPtr(T*& pointer, int base) {
    if (pointer)
        pointer = reinterpret_cast<T*>(reinterpret_cast<int>(pointer) + base);
}

class CSoundTable {
public:
    // Inferred record views; only their 16-bit fields are converted.
    struct SKey {
        void EndianSwap() {
            ChangeEndian(m_values[0]);
            ChangeEndian(m_values[1]);
            ChangeEndian(m_values[2]);
            ChangeEndian(m_values[3]);
        }

        short m_values[4];
    };
    struct SSoundParms {
        void EndianSwap() {
            ChangeEndian(m_values[0]);
            ChangeEndian(m_values[1]);
            ChangeEndian(m_values[2]);
            ChangeEndian(m_values[3]);
            ChangeEndian(m_values[4]);
        }

        unsigned char unknown00[4];
        short m_values[5];
    };
    struct SAnimEntry {
        void EndianSwap() {
            ChangeEndian(m_value0);
            ChangeEndian(m_values[0]);
            ChangeEndian(m_values[1]);
            ChangeEndian(m_values[2]);
            ChangeEndian(m_values[3]);
            ChangeEndian(m_values[4]);
        }

        short m_value0;
        unsigned char unknown02[4];
        short m_values[5];
    };

    // Inferred views of the two loaded headers.
    struct FileView {
        unsigned char unknown00[4];
        int m_counts[5];
        SKey* m_keys;
        SSoundParms* m_parms;
    };
    struct DataView {
        unsigned char unknown00[8];
        int m_counts[3];
        SAnimEntry* m_entries;
    };

    CSoundTable() : m_file(0) {}
    ~CSoundTable();
    void Init(int, int);
    void EndianSwap();

    FileView* m_file;
    DataView* m_data;
};

void CSoundTable::Init(int, int) {
    LevelResourceView* resource = TLT_FindNextResourceByType((TLTResourceID)12, 0);
    m_file = (FileView*)TLT_LoadFileFromLevelBigFile(resource->m_names->m_eventFile, 0);
    m_data = (DataView*)TLT_LoadFileFromLevelBigFile(resource->m_names->m_animFile, 0);
    EndianSwap();
    if (m_file) {
        int base = (int)m_file;
        offsetPtr(m_file->m_keys, base);
        offsetPtr(m_file->m_parms, base);
        int i;
        for (i = 0; i < m_file->m_counts[3]; i++) {
            m_file->m_keys[i].EndianSwap();
        }
        for (i = 0; i < m_file->m_counts[4]; i++) {
            m_file->m_parms[i].EndianSwap();
        }
    }
    if (m_data) {
        offsetPtr(m_data->m_entries, (int)m_data);
        for (int i = 0; i < m_data->m_counts[2]; i++) {
            m_data->m_entries[i].EndianSwap();
        }
    }
}

void CSoundTable::EndianSwap() {
    if (m_file) {
        ChangeEndian(m_file->m_counts[0]);
        ChangeEndian(m_file->m_counts[1]);
        ChangeEndian(m_file->m_counts[2]);
        ChangeEndian(m_file->m_counts[3]);
        ChangeEndian(m_file->m_counts[4]);
        ChangeEndian(m_file->m_keys);
        ChangeEndian(m_file->m_parms);
    }
    if (m_data) {
        ChangeEndian(m_data->m_counts[0]);
        ChangeEndian(m_data->m_counts[1]);
        ChangeEndian(m_data->m_counts[2]);
        ChangeEndian(m_data->m_entries);
    }
}

// The event lookups come first in the original file and are not part of this
// unit. Init's offsetPtr instantiations are emitted after the destructor, weak
// as in the image.

CSoundTable::~CSoundTable() {
    TLT_CloseFile(m_file);
}

CSoundTable g_soundMapSingleton;
