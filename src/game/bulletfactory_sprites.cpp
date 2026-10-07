// A fragment of bulletfactory.cpp (0x800ccda8): CBulletFactory::GetBulletWeight
// (the projectile or thrown property table's first value) and
// DeleteBulletGUISprites, which deletes each property's GUI sprite unless an
// earlier property with the same sprite name already owns it, and
// LoadBulletGUISprites, which shares or creates them (dropping projectile
// sprites whose texture failed to load), and LoadBulletParameters, which loads
// the multiplayer or difficulty's parameter file, allocates both property
// tables and fills them from its typed records, converting each record's byte
// order (the byte-order helpers are the inlined ones described in propdat.cpp,
// inferred; the converted fields are named by offset). The class names
// come from the mangled symbols; the property-table views, the sprite's
// virtual table position (+28), its size and allocator and the factory are
// inferred, non-virtual views where possible.
extern "C" int strcmp(const char*, const char*);
void* DWI_alloc(const char*, int, int);
extern "C" void* memcpy(void*, const void*, unsigned long);
void DebugMsg(const char*, ...);
void TLT_CloseFile(void*);

enum TLTResourceID {};
struct LevelFileContentsStruct_;

// Inferred view of the level resource entry and its file names.
struct BulletFileNamesView {
    unsigned char unknown000[36];
    const char* m_easy;
    unsigned char unknown028[4];
    const char* m_normal;
    unsigned char unknown030[4];
    const char* m_hard;
    unsigned char unknown038[468];
    const char* m_multiplayer;
};

struct LevelResourceView {
    unsigned char unknown00[8];
    BulletFileNamesView* m_names;
};

LevelResourceView* TLT_FindNextResourceByType(TLTResourceID, LevelFileContentsStruct_*);
void* TLT_LoadFileFromLevelBigFile(const char*, int*);

class CShellMenu {
public:
    unsigned char Get_currentDifficulty();

    unsigned char unknown0000[0x19dc];
};

extern CShellMenu g_Shell;
extern bool g_bInMultiplayerMode;

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

inline void ChangeEndian(unsigned short& value) {
    ChangeEndian(*reinterpret_cast<short*>(&value));
}

template <class T> inline void ChangeEndian(T& value) {
    ChangeEndian(*reinterpret_cast<int*>(&value));
}


enum ERenderPriority {};

class CSpriteBase {
public:
    unsigned char unknown00[28];
};

class CSprite : public CSpriteBase {
public:
    enum ESpriteLoadType {};

    static void* operator new(unsigned long size) { return DWI_alloc(0, size, 1024); }
    CSprite(const char*, ESpriteLoadType, ERenderPriority);
    virtual ~CSprite();

    void* m_texture;
    unsigned char unknown24[64];
};

struct ProjectilePropertiesView {
    float weight;
    unsigned char unknown04[20];
    char spriteName[20];
    CSprite* sprite;
    unsigned char unknown30[16];
};

struct ThrownPropertiesView {
    float weight;
    unsigned char unknown04[20];
    char spriteName[20];
    CSprite* sprite;
    unsigned char unknown30[28];
};

class CBulletFactory {
public:
    float GetBulletWeight(bool, int);
    void DeleteBulletGUISprites();
    void LoadBulletGUISprites();
    void LoadBulletParameters();

    ProjectilePropertiesView* m_projectileProperties;
    ThrownPropertiesView* m_thrownProperties;
};

float CBulletFactory::GetBulletWeight(bool projectile, int type) {
    if (projectile)
        return m_projectileProperties[type].weight;
    return m_thrownProperties[type].weight;
}

void CBulletFactory::DeleteBulletGUISprites() {
    int i;
    int j;
    for (i = 0; i < 31; i++) {
        char* name = m_projectileProperties[i].spriteName;
        if (name[0]) {
            for (j = 0; j < i; j++) {
                if (strcmp(name, m_projectileProperties[j].spriteName) == 0) {
                    m_projectileProperties[i].sprite = 0;
                    break;
                }
            }
            if (m_projectileProperties[i].sprite) {
                delete m_projectileProperties[i].sprite;
                m_projectileProperties[i].sprite = 0;
            }
        }
    }
    for (i = 0; i < 11; i++) {
        char* name = m_thrownProperties[i].spriteName;
        if (name[0]) {
            for (j = 0; j < i; j++) {
                if (strcmp(name, m_thrownProperties[j].spriteName) == 0) {
                    m_thrownProperties[i].sprite = 0;
                    break;
                }
            }
            if (m_thrownProperties[i].sprite) {
                delete m_thrownProperties[i].sprite;
                m_thrownProperties[i].sprite = 0;
            }
        }
    }
}

void CBulletFactory::LoadBulletGUISprites() {
    int i;
    int j;
    for (i = 0; i < 31; i++) {
        m_projectileProperties[i].sprite = 0;
        char* name = m_projectileProperties[i].spriteName;
        if (name[0]) {
            for (j = 0; j < i; j++) {
                if (strcmp(name, m_projectileProperties[j].spriteName) == 0) {
                    m_projectileProperties[i].sprite = m_projectileProperties[j].sprite;
                    break;
                }
            }
            if (!m_projectileProperties[i].sprite) {
                m_projectileProperties[i].sprite = new CSprite(name, (CSprite::ESpriteLoadType)1, (ERenderPriority)111);
                if (!m_projectileProperties[i].sprite->m_texture)
                    m_projectileProperties[i].sprite = 0;
            }
        }
    }
    for (i = 0; i < 11; i++) {
        m_thrownProperties[i].sprite = 0;
        char* name = m_thrownProperties[i].spriteName;
        if (name[0]) {
            for (j = 0; j < i; j++) {
                if (strcmp(name, m_thrownProperties[j].spriteName) == 0) {
                    m_thrownProperties[i].sprite = m_thrownProperties[j].sprite;
                    break;
                }
            }
            if (!m_thrownProperties[i].sprite)
                m_thrownProperties[i].sprite = new CSprite(name, (CSprite::ESpriteLoadType)1, (ERenderPriority)111);
        }
    }
}

void CBulletFactory::LoadBulletParameters() {
    short offset;
    char* file;
    LevelResourceView* resource = TLT_FindNextResourceByType((TLTResourceID)11, 0);
    if (g_bInMultiplayerMode) {
        file = (char*)TLT_LoadFileFromLevelBigFile(resource->m_names->m_multiplayer, 0);
    } else {
        switch (g_Shell.Get_currentDifficulty()) {
        case 1:
            file = (char*)TLT_LoadFileFromLevelBigFile(resource->m_names->m_easy, 0);
            break;
        case 2:
            file = (char*)TLT_LoadFileFromLevelBigFile(resource->m_names->m_normal, 0);
            break;
        case 3:
            file = (char*)TLT_LoadFileFromLevelBigFile(resource->m_names->m_hard, 0);
            break;
        default:
            DebugMsg("Unknown difficulty level.  Assuming Level TE\n");
            file = (char*)TLT_LoadFileFromLevelBigFile(resource->m_names->m_easy, 0);
            break;
        }
    }
    int numProjectiles;
    int numThrown;
    memcpy(&numProjectiles, file + 4, 4);
    memcpy(&numThrown, file + 8, 4);
    offset = 12;
    ChangeEndian(numProjectiles);
    ChangeEndian(numThrown);
    m_projectileProperties = (ProjectilePropertiesView*)new char[numProjectiles * 64];
    m_thrownProperties = (ThrownPropertiesView*)new char[numThrown * 76];
    for (int read = 0; read < numThrown + numProjectiles;) {
        short type;
        memcpy(&type, file + offset, 2);
        offset += 2;
        ChangeEndian(type);
        switch (type) {
        case 1:
            offset += 82;
            break;
        case 2: {
            short index;
            memcpy(&index, file + offset, 2);
            ChangeEndian(index);
            memcpy(&m_projectileProperties[index], file + (short)(offset + 2), 64);
            offset += 66;
            read++;
            ChangeEndian(*(unsigned int*)&m_projectileProperties[index].sprite);
            ChangeEndian(*(short*)((char*)&m_projectileProperties[index] + 52));
            ChangeEndian(*(unsigned int*)&m_projectileProperties[index].weight);
            ChangeEndian(*(unsigned int*)((char*)&m_projectileProperties[index] + 48));
            ChangeEndian(*(unsigned int*)((char*)&m_projectileProperties[index] + 56));
            break;
        }
        case 3: {
            short index;
            memcpy(&index, file + offset, 2);
            ChangeEndian(index);
            memcpy(&m_thrownProperties[index], file + (short)(offset + 2), 76);
            ChangeEndian(*(unsigned int*)&m_thrownProperties[index].sprite);
            ChangeEndian(*(short*)((char*)&m_thrownProperties[index] + 72));
            ChangeEndian(*(unsigned int*)&m_thrownProperties[index].weight);
            ChangeEndian(*(unsigned int*)((char*)&m_thrownProperties[index] + 48));
            ChangeEndian(*(unsigned int*)((char*)&m_thrownProperties[index] + 52));
            ChangeEndian(*(unsigned int*)((char*)&m_thrownProperties[index] + 56));
            ChangeEndian(*(unsigned int*)((char*)&m_thrownProperties[index] + 60));
            ChangeEndian(*(unsigned int*)((char*)&m_thrownProperties[index] + 64));
            ChangeEndian(*(unsigned int*)((char*)&m_thrownProperties[index] + 68));
            offset += 78;
            read++;
            break;
        }
        }
    }
    TLT_CloseFile(file);
}
