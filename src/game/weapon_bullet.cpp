// A fragment of weapon.cpp (0x800d4fac): CWeapon::SetInfiniteAmo (a flag
// bit), CanCook and GetBulletSpeed (asking the bullet factory about the
// weapon's bullet type; the speed is the weapon's speed scaled by the bullet
// weight) and GetWeaponSoundType. The file name is this project's; the
// original record is weapon.cpp; GetMuzzleFlashStatus before these uses a
// pooled .sdata2 constant and FireBullet after them is not reconstructed. The
// classes and globals are named by the mangled symbols; CWeapon (as in
// weapon.cpp) and the weapon properties are inferred views (members at their
// offsets, names not original), and the result types are inferred.
// CVector3 view: four floats, 8-byte aligned, overlaid with two doubles. The
// union is inferred from the code, not the original declaration: the position
// is copied as two lfd/stfd pairs, which an implicit copy through the double
// pair reproduces.
class CVector3 {
public:
    union {
        struct {
            float x;
            float y;
            float z;
            float w;
        };
        double pair[2];
    };
} __attribute__((aligned(8)));

struct WeaponPropertiesView {
    unsigned char unknown00[8];
    float speed;
    short bulletType;
    short bulletFlag;
    unsigned char unknown10[56];
    short soundType;
};

class CStaticObject {
public:
    void Init();
};

class CBulletFactory {
public:
    bool CanBulletTypeCook(bool, int);
    float GetBulletWeight(bool, int);
};

extern CBulletFactory* g_pBulletFactory;

class CWeapon {
public:
    CWeapon* AsWeaponObject();
    void GetPosition(CVector3&) const;
    void Init();
    void SetInfiniteAmo(bool);
    bool CanCook() const;
    int GetWeaponSoundType() const;
    float GetBulletSpeed() const;

    unsigned char unknown000[640];
    WeaponPropertiesView* m_properties;
    short m_reserve;
    short m_loaded;
    unsigned char unknown288[4];
    int m_reloadState;
    unsigned char unknown290[32];
    CVector3 m_position;
    unsigned char reloading : 1;
    unsigned char unknown2c0b : 2;
    unsigned char infiniteAmmo : 1;
    unsigned char unknown2c0c : 4;
};

void CWeapon::SetInfiniteAmo(bool infinite) {
    infiniteAmmo = infinite;
}

bool CWeapon::CanCook() const {
    return g_pBulletFactory->CanBulletTypeCook(m_properties->bulletFlag != 0, m_properties->bulletType);
}

int CWeapon::GetWeaponSoundType() const {
    return m_properties->soundType;
}

float CWeapon::GetBulletSpeed() const {
    float weight = g_pBulletFactory->GetBulletWeight(m_properties->bulletFlag != 0, m_properties->bulletType);
    return m_properties->speed * weight;
}
