// The end of world_object.cpp (0x800b82a0): the weak memory-factory
// destructor (owned memory freed, its words cleared), the two filename
// wrappers that read a compartment's CDB and CPT names from the big file, the
// static initialisation of the compartment and CDB filename tables (cleared;
// destructors registered) and the weak filename-table destructor emitted
// after it (its names array deleted). The weak compartment-list and node
// destructors before these are not part of this unit (the node destructor's
// out-of-line copy is not reproduced). The classes, functions and globals are
// named by the symbols; the members are inferred.

extern "C" void MEM_free(void*);
void GetCompartmentCDBNameFromIndex(int, char*, int);
void GetCompartmentCPTNameFromIndex(int, char*, int);

namespace dwi {
class CMemFactory {
public:
    ~CMemFactory();

    void* m_memory;
    int data04;
    int data08;
    int data0c;
    int data10;
    bool m_owned;
};

__declspec(weak) CMemFactory::~CMemFactory() {
    if (m_owned)
        MEM_free(m_memory);
    m_memory = 0;
    data04 = 0;
    data08 = 0;
    data0c = 0;
    data10 = 0;
}
}

void GetCdbFilenameFromBigFile(int index, char* name, int size) {
    GetCompartmentCDBNameFromIndex(index, name, size);
}

void GetCptFilenameFromBigFile(int index, char* name, int size) {
    GetCompartmentCPTNameFromIndex(index, name, size);
}

class CFilenameTable {
public:
    CFilenameTable() : m_count(0), m_names(0) {}
    ~CFilenameTable() { delete[] m_names; }

    int m_count;
    char* m_names;
};

CFilenameTable g_CptFnameTable;
CFilenameTable g_CdbFnameTable;
