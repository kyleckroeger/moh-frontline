// CTankObject::MapWeaponSlot and AddWeapon (a fragment of Tank_object.cpp):
// map a sub-object to a weapon slot, and create a slot's weapon from its CRC
// through the weapon factory (the empty-weapon CRC clears the slot), choosing
// the weapon's mode from its properties. The names come from the mangled
// symbols; the members, the result type and the weapon's fields are inferred
// from offsets (the factory has its symbol's 28-byte size), and the classes are non-virtual views.
class ISceneNode;

/* inferred views */
struct WEAPONPROPSVIEW {
    unsigned char unknown00[14];
    short field0E;
};

class CWeapon {
public:
    unsigned char unknown000[640];
    WEAPONPROPSVIEW* m_properties;
    unsigned char unknown284[16];
    int m_mode;
};

class CWeaponFactory {
public:
    CWeapon* CreateWeaponByCRC(int, ISceneNode*, int);

    unsigned char unknown00[28];
};

extern CWeaponFactory g_WeaponFactory;

class CTankObject {
public:
    void MapWeaponSlot(int, int);
    bool AddWeapon(int, int);

    unsigned char unknown000[1948];
    int m_weaponSlot[8];
    CWeapon* m_weapons[8];
};

void CTankObject::MapWeaponSlot(int subObject, int slot) {
    m_weaponSlot[subObject] = slot;
}

bool CTankObject::AddWeapon(int slot, int crc) {
    if (crc == (int)0xfb9630c9) {
        m_weapons[slot] = 0;
        return false;
    }
    m_weapons[slot] = g_WeaponFactory.CreateWeaponByCRC(crc, (ISceneNode*)this, 999);
    if (m_weapons[slot]) {
        if (m_weapons[slot]->m_properties->field0E)
            m_weapons[slot]->m_mode = 4;
        else
            m_weapons[slot]->m_mode = 10;
        return true;
    }
    return false;
}
