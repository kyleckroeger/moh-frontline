// A fragment of weapon.cpp (0x800d4f28): CWeapon's weak AsWeaponObject
// (returns itself), GetPosition (the position at +688) and Init (the static
// object's Init), then GetMuzzleFlashStatus (once the flash time at +708
// passes the duration at +712, clears two flag bits, the flash bit twice, and
// the time; returns the flash bit; 0.0f is an entry of the file's .sdata2
// pool). The file name is this project's; the original record is weapon.cpp
// and the functions before these are not reconstructed. The
// classes are named by the mangled symbols; CWeapon (as in weapon.cpp) and
// the weapon properties are inferred views (members at their offsets, names
// not original), as are the flag and time members and the result type. The
// first three are inline in the original (weak symbols), so they are defined
// __declspec(weak).
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
    unsigned char GetMuzzleFlashStatus();

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
    unsigned char flashFlag : 1;
    unsigned char muzzleFlash : 1;
    unsigned char unknown2c0e : 2;
    unsigned char unknown2c1[3];
    float m_flashTime;
    float m_flashDuration;
};

__declspec(weak) CWeapon* CWeapon::AsWeaponObject() {
    return this;
}

__declspec(weak) void CWeapon::GetPosition(CVector3& v) const {
    v = m_position;
}

__declspec(weak) void CWeapon::Init() {
    ((CStaticObject*)this)->Init();
}

unsigned char CWeapon::GetMuzzleFlashStatus() {
    if (m_flashTime > m_flashDuration) {
        flashFlag = 0;
        muzzleFlash = 0;
        muzzleFlash = 0;
        m_flashTime = 0.0f;
    }
    return muzzleFlash;
}
