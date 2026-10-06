// CWeapon ammunition helpers: loading a single round from the reserve into
// the clip (up to the clip size from the weapon's properties), starting a
// reload and emptying the weapon. CWeapon is named by the mangled symbols; the
// members, the property record and the flag-byte bit-field view are inferred.
// The rest of the file is not part of this unit.
struct WeaponPropertiesView {
    unsigned char unknown00[4];
    short clipSize;
};

class CWeapon {
public:
    void ReloadSingle();
    void StartReloading();
    void Empty();

    unsigned char unknown000[640];
    WeaponPropertiesView* m_properties;
    short m_reserve;
    short m_loaded;
    unsigned char unknown288[4];
    int m_reloadState;
    unsigned char unknown290[48];
    unsigned char reloading : 1;
    unsigned char flags : 7;
};

void CWeapon::ReloadSingle() {
    if (m_loaded < m_properties->clipSize && m_reserve > 0) {
        m_loaded++;
        m_reserve--;
    }
}

void CWeapon::StartReloading() {
    m_reloadState = 1;
    reloading = 1;
}

void CWeapon::Empty() {
    m_reserve = 0;
    m_loaded = 0;
}
