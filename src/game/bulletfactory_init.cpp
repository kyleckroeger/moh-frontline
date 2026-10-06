// A fragment of bulletfactory.cpp (0x800cd8e8): CBulletFactory's Shutdown
// (deletes the GUI sprites, shuts both bullet helpers down, frees the two
// property arrays and clears the initialised flag), Reset (resets both
// helpers) and Init (loads the bullet parameters and GUI sprites, initialises
// each helper with its property array and sets the flag). The file name is
// this project's; the original record is bulletfactory.cpp and the destructor
// after these is not reconstructed. The classes, the helper template and the
// property and type names come from the mangled symbols; CBulletFactory is an
// inferred non-virtual view (members at their offsets, names not original).
class CProjectileBullet;
class CThrownBullet;
struct ProjectileBulletProperties_struct;
struct ThrownBulletProperties_struct;
enum EProjectileBulletTypes {};
enum EThrownBulletTypes {};

template <class Bullet, class Properties, class Types>
class CBulletFactoryHelper {
public:
    void Init(Properties*);
    void Reset();
    void Shutdown();

    unsigned char unknown00[20];
};

class CBulletFactory {
public:
    void Shutdown();
    void Reset();
    void Init();
    void DeleteBulletGUISprites();
    void LoadBulletParameters();
    void LoadBulletGUISprites();

    ProjectileBulletProperties_struct* m_projectileProperties;
    ThrownBulletProperties_struct* m_thrownProperties;
    CBulletFactoryHelper<CProjectileBullet, ProjectileBulletProperties_struct, EProjectileBulletTypes> m_projectiles;
    CBulletFactoryHelper<CThrownBullet, ThrownBulletProperties_struct, EThrownBulletTypes> m_thrown;
    bool m_initialised;
};

void CBulletFactory::Shutdown() {
    DeleteBulletGUISprites();
    m_projectiles.Shutdown();
    m_thrown.Shutdown();
    delete[] m_thrownProperties;
    delete[] m_projectileProperties;
    m_thrownProperties = 0;
    m_projectileProperties = 0;
    m_initialised = false;
}

void CBulletFactory::Reset() {
    m_projectiles.Reset();
    m_thrown.Reset();
}

void CBulletFactory::Init() {
    LoadBulletParameters();
    LoadBulletGUISprites();
    m_projectiles.Init(m_projectileProperties);
    m_thrown.Init(m_thrownProperties);
    m_initialised = true;
}
