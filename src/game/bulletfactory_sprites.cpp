// A fragment of bulletfactory.cpp (0x800ccda8): CBulletFactory::GetBulletWeight
// (the projectile or thrown property table's first value) and
// DeleteBulletGUISprites, which deletes each property's GUI sprite unless an
// earlier property with the same sprite name already owns it, and
// LoadBulletGUISprites, which shares or creates them (dropping projectile
// sprites whose texture failed to load). The class names
// come from the mangled symbols; the property-table views, the sprite's
// virtual table position (+28), its size and allocator and the factory are
// inferred, non-virtual views where possible.
extern "C" int strcmp(const char*, const char*);
void* DWI_alloc(const char*, int, int);

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
