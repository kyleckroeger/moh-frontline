// CConfigFile: a loaded text file split into "[section]" blocks, with
// "key = value" lookups in the current section. CConfigFile and
// CConfigSection are named by the mangled symbols; members are inferred from
// offsets and are not original.
extern "C" {
char* strchr(const char*, int);
int stricmp(const char*, const char*);
}

void* TLT_LoadFileNormal(const char*, int*, int);
void TLT_CloseFile(void*);
void DebugMsg(const char*, ...);

const char* eolString = "\r\n";

class CConfigSection {
public:
    CConfigSection() : m_name(0), m_end(0) {}

    char* m_name;
    char* m_start;
    char* m_end;
};

class CConfigFile {
public:
    CConfigFile(const char*);
    ~CConfigFile();
    bool SetSection(const char*);
    bool GetBool(const char*, bool);
    bool GetString(const char*, char*, int);

    CConfigSection m_sections[64];
    int m_count;
    int m_current;
    char* m_data;
};

// GetString and GetBool come first in the original file. They share an
// inlined "key = value" lookup and are drafted in scratch but not matched, so
// this unit starts at SetSection.

bool CConfigFile::SetSection(const char* name) {
    CConfigSection* section = m_sections;

    for (int i = 0; i < m_count; section++, i++) {
        if (stricmp(section->m_name, name) == 0) {
            m_current = i;
            return true;
        }
    }
    m_current = -1;
    return false;
}

CConfigFile::~CConfigFile() {
    if (m_data)
        TLT_CloseFile(m_data);
}

CConfigFile::CConfigFile(const char* filename) {
    int size;
    char* p;

    m_count = 0;
    m_current = -1;
    m_data = 0;
    m_data = (char*)TLT_LoadFileNormal(filename, &size, 0);
    if (!m_data || size < 1)
        return;
    m_data[size - 1] = '\n';
    p = strchr(m_data, '[');
    while (p) {
        char* end = strchr(++p, ']');
        if (!end) {
            DebugMsg("CConfigFile::CConfigFile() -- Section name %s doesn't terminate ( ] )", p);
            break;
        }
        m_sections[m_count].m_name = p;
        *end = 0;
        m_sections[m_count].m_start = end + 1;
        p = m_sections[m_count].m_end = strchr(end + 1, '[');
        if (p)
            m_sections[m_count].m_end--;
        else
            m_sections[m_count].m_end = m_data + size;
        m_count++;
    }
}
