// A fragment of player.cpp (0x800a6300): CPlayerObject::InitClass (records the
// screen and camera and clears the bazooka overlay), CleanUp and InitAI
// (forwarded to the AI doodad at +36) and ReInit (destroys the three sprites
// of each of the 45 weapon objects at +1120, then runs Init). The file-local
// statics are declared extern here; the member offsets are inferred and the
// views are non-virtual.
class CScreen;
class CCamera;
class CPlayerObject;

class CSprite {
public:
    ~CSprite();
};

class CAIPlayerDoodad {
public:
    void CleanUp();
    void Init(CPlayerObject*);
};

class CPlayerWeaponObject {
public:
    unsigned char unknown0000[12520];
    CSprite* m_sprites[3];
};

class CPlayerObject {
public:
    static void InitClass(CScreen*, CCamera*);
    void CleanUp();
    void InitAI();
    void ReInit();
    void Init();

    unsigned char unknown000[36];
    CAIPlayerDoodad m_aiDoodad;
    unsigned char unknown025[1083];
    CPlayerWeaponObject* m_weapons[45];
};

// File-local in the original.
extern CScreen* g_pScreen;
extern CCamera* g_pCamera;
extern CSprite* g_pBazookaOverlay;

void CPlayerObject::InitClass(CScreen* screen, CCamera* camera) {
    g_pScreen = screen;
    g_pCamera = camera;
    g_pBazookaOverlay = 0;
}

void CPlayerObject::CleanUp() {
    m_aiDoodad.CleanUp();
}

void CPlayerObject::InitAI() {
    m_aiDoodad.Init(this);
}

void CPlayerObject::ReInit() {
    for (int i = 0; i < 45; i++) {
        if (m_weapons[i]) {
            if (m_weapons[i]->m_sprites[0])
                m_weapons[i]->m_sprites[0]->~CSprite();
            if (m_weapons[i]->m_sprites[1])
                m_weapons[i]->m_sprites[1]->~CSprite();
            if (m_weapons[i]->m_sprites[2])
                m_weapons[i]->m_sprites[2]->~CSprite();
        }
    }
    Init();
}
