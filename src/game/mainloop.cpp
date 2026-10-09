// The static initialisation of mainloop.cpp (0x8007b898): the global
// animated-object, bullet and weapon factories and the bullet-decal manager
// are built through their constructors, the last three with their
// destructors registered (the animated-object factory has none). The player
// table between them needs no construction. The game-loop functions before it
// are not reconstructed. The classes and globals are named by the symbols;
// the object sizes come from the symbols and the members are opaque here.
class CPlayerObject;

class CAnimObjectFactory {
public:
    CAnimObjectFactory();

    unsigned char unknown00[28];
};

class CBulletFactory {
public:
    CBulletFactory();
    ~CBulletFactory();

    unsigned char unknown00[52];
};

class CWeaponFactory {
public:
    CWeaponFactory();
    ~CWeaponFactory();

    unsigned char unknown00[28];
};

class CBulletDecalManager {
public:
    CBulletDecalManager();
    ~CBulletDecalManager();

    unsigned char unknown000[1484];
};

CAnimObjectFactory g_AnimObjFactory;
CPlayerObject* g_pPlayers[4];
CBulletFactory g_BulletFactory;
CWeaponFactory g_WeaponFactory;
CBulletDecalManager g_BulletDecalManager;
