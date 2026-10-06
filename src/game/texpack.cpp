// A texture pack: a name table sorted for bsearch and a parallel table of
// textures, both stored as offsets from the pack and fixed up on construction.
class ShapeFile;

class CTexture {
public:
    void* shape;

    void Set(ShapeFile*, bool);
};

class CTexturePack {
public:
    CTexture* FindTexture(char*);
    CTexturePack();

    unsigned char unknown00[4];
    unsigned int count;
    char (*names)[16];
    CTexture** textures;
};

extern "C" {
void* bsearch(const void*, const void*, unsigned long, unsigned long, int (*)(const void*, const void*));
int stricmp(const char*, const char*);
}
void DebugMsg(const char*, ...);

template <class T> void offsetPtr(T*& pointer, int base) {
    if (pointer)
        pointer = reinterpret_cast<T*>(reinterpret_cast<int>(pointer) + base);
}

// The linked image keeps one copy of offsetPtr<void> (emitted with
// propdat.cpp); this file's own weak copy was dropped by the linker, so the
// instantiation is declared here rather than generated.
template <> void offsetPtr<void>(void*&, int);

static int _tex_compare(const char*, const char*);

CTexture* CTexturePack::FindTexture(char* name) {
    char (*entry)[16] = static_cast<char (*)[16]>(
        bsearch(name, names, count, 16, reinterpret_cast<int (*)(const void*, const void*)>(_tex_compare)));
    if (entry)
        return textures[entry - names];
    DebugMsg("WARNING! Could not find \"%s\" in texture pack.\n", name);
    return 0;
}

CTexturePack::CTexturePack() {
    offsetPtr(names, reinterpret_cast<int>(this));
    offsetPtr(textures, reinterpret_cast<int>(this));
    for (unsigned int i = 0; i < count; i++) {
        offsetPtr(textures[i], reinterpret_cast<int>(this));
        if (textures[i]->shape)
            offsetPtr(textures[i]->shape, reinterpret_cast<int>(this));
        textures[i]->Set(static_cast<ShapeFile*>(textures[i]->shape), false);
    }
}

static int _tex_compare(const char* a, const char* b) {
    return stricmp(a, b);
}
