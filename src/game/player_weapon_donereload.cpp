// A fragment of player_weapon_object.cpp (0x800a800c):
// CPlayerWeaponObject::DoneReloading forwards to the weapon. Shoot after this
// uses the file's int-to-float .sdata2 constant and is not part of this unit.
// CPlayerWeaponObject, CWeapon, CPlayerObject and EDumpType are named by the
// mangled symbols; the members, the weapon property record and the flag bits
// are inferred views. The rest of the file is not part of this unit.
enum EDumpType {};

void EnableDumpCollisionsByType(EDumpType, int, bool);

struct WeaponPropertiesView {
    unsigned char unknown00[18];
    short maxReserve;
};

class CWeapon {
public:
    void AddAmo(short);
    void DoneReloading();

    unsigned char unknown000[640];
    WeaponPropertiesView* m_properties;
    short m_reserve;
    short m_loaded;
    unsigned char unknown288[16];
    int m_type;
};

class CPlayerObject {
public:
    unsigned char unknown000[919];
    unsigned char m_flag919 : 1;
    unsigned char unknown397 : 7;
};

class CPlayerWeaponObject {
public:
    bool IsDrawEnabled() const;
    void AddAmo(int);
    void DoneReloading();
    bool IsSelectable() const;

    unsigned char unknown0000[12512];
    CPlayerObject* m_player;
    CWeapon* m_weapon;
    unsigned char unknown30e8[234];
    unsigned char m_flag0 : 1;
    unsigned char m_drawEnabled : 1;
    bool m_selectable : 1;
    unsigned char m_flags : 5;
};

void CPlayerWeaponObject::DoneReloading() {
    m_weapon->DoneReloading();
}
