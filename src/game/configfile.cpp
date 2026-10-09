// CConfigFile: a loaded text file split into "[section]" blocks, with
// "key = value" lookups in the current section (GetString copies the value up
// to the end of its line, GetBool compares its first four characters with
// "true"; both find the value through the inline FindValue). CConfigFile and
// CConfigSection are named by the mangled symbols; members and the FindValue
// helper are inferred and are not original.
extern "C" {
void* memcpy(void*, const void*, unsigned long);
char* strncpy(char*, const char*, unsigned long);
char* strchr(const char*, int);
char* strstr(const char*, const char*);
int stricmp(const char*, const char*);
extern unsigned char __ctype_map[];
}

// MSL's isspace macro: the whitespace bits (0x06) of the character class map.
#define isspace(c) (__ctype_map[(unsigned char)(c)] & 0x06)

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

    char* FindValue(const char* key) {
        if (m_current < 0)
            return 0;
        char* entry = strstr(m_sections[m_current].m_start, key);
        char* eol;
        if (entry) {
            char* found = strstr(entry, eolString);
            eol = found;
            if (found == 0)
                eol = m_sections[m_current].m_end;
            char* equals = strchr(entry, '=');
            if (equals && equals < eol) {
                char* value;
                for (value = equals + 1; isspace(*value); value++)
                    ;
                return value;
            }
        }
        return 0;
    }

    CConfigSection m_sections[64];
    int m_count;
    int m_current;
    char* m_data;
};

bool CConfigFile::GetString(const char* key, char* buffer, int size) {
    char* value = FindValue(key);

    if (value) {
        memcpy(buffer, value, size);
        char* eol = strstr(buffer, eolString);
        if (eol)
            *eol = 0;
        return true;
    }
    return false;
}

bool CConfigFile::GetBool(const char* key, bool defaultValue) {
    char* value = FindValue(key);

    if (value) {
        char text[5];

        strncpy(text, value, 4);
        text[4] = 0;
        if (stricmp(text, "true") == 0)
            return true;
        return false;
    }
    return defaultValue;
}

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
