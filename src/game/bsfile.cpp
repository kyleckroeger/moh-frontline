// Behaviour-script class files, the functions at the end of the file: looking
// up a loaded class by name (reading it again if it is not yet loaded, or
// reading and linking a new one), closing a class's two files, and the empty
// initialisation and memory-requirement hooks. BSClass_struct is named by the
// mangled symbols; its members are inferred from offsets. BSFileReadClass and
// the rest of the file are not part of this unit.
extern "C" int stricmp(const char*, const char*);
void TLT_CloseFile(void*);

struct BSClass_struct {
    unsigned char unknown00[4];
    BSClass_struct* next;
    char name[68];
    void* codeFile;
    void* classFile;
    unsigned char unknown54[6];
    bool loaded;
};

int BSFileReadClass(const char*, bool, bool, BSClass_struct**);

extern BSClass_struct* g_pcClassList;

// The name search is inlined in the original; this helper and its name are
// inferred.
static inline BSClass_struct* BSFileFindClass(const char* name) {
    for (BSClass_struct* entry = g_pcClassList; entry; entry = entry->next) {
        if (stricmp(name, entry->name) == 0)
            return entry;
    }
    return 0;
}

int BSFileGetClass(const char* name, BSClass_struct** result) {
    *result = BSFileFindClass(name);
    if (*result) {
        if ((*result)->loaded)
            return 0;
        return BSFileReadClass(name, true, false, result);
    }
    BSFileReadClass(name, true, true, result);
    BSClass_struct* entry = *result;
    entry->next = g_pcClassList;
    g_pcClassList = entry;
    return 0;
}

void BSFileDeleteClass(BSClass_struct* entry) {
    TLT_CloseFile(entry->classFile);
    TLT_CloseFile(entry->codeFile);
}

int BSInitFile() {
    return 0;
}

int BSFileGetMemoryRequirements() {
    return 0;
}
