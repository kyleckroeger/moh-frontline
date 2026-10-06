// A fragment of player.cpp (0x8009f464): CPlayerObject's current-weapon
// queries. While the mounted-weapon flag is set they read the mounted weapon;
// otherwise the selected slot's weapon object's weapon, with 0 (or null) when
// no slot is selected: the reserve and clip ammo counts and the bullet sprite.
// GetWeaponByCRC returns the weapon object in the slot GetWeaponInfoByCRC
// finds. The file name is this project's; the original record is player.cpp
// and the camera-shake functions before these (pooled .sdata2 constants) and
// GetWeaponInfoByCRC after them are not reconstructed. The classes and
// functions are named by the mangled symbols; CPlayerObject,
// CPlayerWeaponObject and CWeapon are inferred non-virtual views (members at
// their offsets, names not original) and the result types are inferred.
class CSprite;

class CWeapon {
public:
    CSprite* GetBulletSprite() const;

    unsigned char unknown000[644];
    short m_reserveAmmo;
    short m_clipAmmo;
};

class CPlayerWeaponObject {
public:
    unsigned char unknown0000[12516];
    CWeapon* m_weapon;
};

class CPlayerObject {
public:
    int GetCurrentWeaponReserveAmoCount() const;
    int GetCurrentWeaponClipAmoCount() const;
    CSprite* GetCurrentBulletSprite() const;
    CPlayerWeaponObject* GetWeaponByCRC(int) const;
    static int GetWeaponInfoByCRC(int, int*, int*);

    unsigned char unknown000[918];
    unsigned char m_usingMountedWeapon : 1;
    unsigned char unknown396b : 7;
    unsigned char unknown397[197];
    int m_currentWeapon;
    CPlayerWeaponObject* m_weapons[96];
    CWeapon* m_mountedWeapon;
};

int CPlayerObject::GetCurrentWeaponReserveAmoCount() const {
    if (m_usingMountedWeapon)
        return m_mountedWeapon->m_reserveAmmo;
    if (m_currentWeapon >= 0)
        return m_weapons[m_currentWeapon]->m_weapon->m_reserveAmmo;
    return 0;
}

int CPlayerObject::GetCurrentWeaponClipAmoCount() const {
    if (m_usingMountedWeapon)
        return m_mountedWeapon->m_clipAmmo;
    if (m_currentWeapon >= 0)
        return m_weapons[m_currentWeapon]->m_weapon->m_clipAmmo;
    return 0;
}

CSprite* CPlayerObject::GetCurrentBulletSprite() const {
    if (m_usingMountedWeapon)
        return m_mountedWeapon->GetBulletSprite();
    if (m_currentWeapon >= 0)
        return m_weapons[m_currentWeapon]->m_weapon->GetBulletSprite();
    return 0;
}

CPlayerWeaponObject* CPlayerObject::GetWeaponByCRC(int crc) const {
    return m_weapons[GetWeaponInfoByCRC(crc, 0, 0)];
}
