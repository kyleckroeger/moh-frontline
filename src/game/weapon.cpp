// CWeapon drawing, update and ammunition helpers: drawing as a static object
// when shown and its holder (+724) does not draw it, updating the proximity
// trigger (+728) before the static-object update when flagged, finishing a
// reload (a full clip when the infinite-ammunition flag is set, else as much
// as the reserve allows), loading a single round from the reserve into the
// clip (up to the clip size from the weapon's properties), starting a reload
// and emptying the weapon, adding ammunition (starting a reload when the
// weapon was empty; capped at the property maximum), recording a shot, the
// bullet sprite from the bullet factory, an empty Shutdown, and Init (the
// static-object part initialised, then the properties, holder, type and
// defaults set, the reserve capped at the property maximum and a reload
// finished as DoneReloading does, the holder-type-specific value at +660
// and the muzzle flash chosen by weapon type; the switch's jump table is
// the unit's .data). CWeapon,
// CStaticObject, CDrawContext, BSGO_Basic, CBulletFactory and
// EWeaponShootType are named by the mangled symbols; the members, the
// holder's virtual slots (the type query's name too), the property record
// and the flag-byte bit-field view are inferred (CWeapon is a non-virtual view derived from a CStaticObject
// view).
// The rest of the file is not part of this unit.
class CDrawContext;
struct BSGO_Basic;

void ForceUpdateProximityTriggerStatus(BSGO_Basic*, bool);

class CWeaponHolderView {
public:
    virtual void unknown08();
    virtual void unknown0c();
    virtual void unknown10();
    virtual void unknown14();
    virtual void unknown18();
    virtual void unknown1c();
    virtual void unknown20();
    virtual void unknown24();
    virtual void unknown28();
    virtual void unknown2c();
    virtual void unknown30();
    virtual void unknown34();
    virtual void unknown38();
    virtual void unknown3c();
    virtual void unknown40();
    virtual void unknown44();
    virtual void unknown48();
    virtual void unknown4c();
    virtual void unknown50();
    virtual void unknown54();
    virtual unsigned int GetWeaponHolderType(); /* name and result type inferred */
    virtual void unknown5c();
    virtual void unknown60();
    virtual void unknown64();
    virtual void unknown68();
    virtual void unknown6c();
    virtual void unknown70();
    virtual void unknown74();
    virtual void unknown78();
    virtual void unknown7c();
    virtual void unknown80();
    virtual void unknown84();
    virtual void unknown88();
    virtual void unknown8c();
    virtual void unknown90();
    virtual void unknown94();
    virtual void unknown98();
    virtual void* DrawsWeapon();
};

class CStaticObject {
public:
    void Init();
    void Draw(CDrawContext&);
    void BeginUpdate(float);
};
struct WeaponPropertiesView {
    unsigned char unknown00[4];
    short clipSize;
    unsigned char unknown06[6];
    short bulletSprite;
    short bulletSpriteFlag;
    unsigned char unknown10[2];
    short maxReserve;
    unsigned char unknown14[20];
    void* unknown28;
};

enum EWeaponTypes {};
struct WeaponProperties_struct;
class ISceneNode;
extern void* g_pStaticObjectMuzzleFlashNPC;

enum EWeaponShootType {};

class CBulletFactory {
public:
    void* GetBulletSprite(bool, int);
};

extern CBulletFactory* g_pBulletFactory;

class CWeapon : public CStaticObject {
public:
    void Draw(CDrawContext&);
    void BeginUpdate(float);
    void DoneReloading();
    void ReloadSingle();
    void AddAmo(short);
    void Shoot(EWeaponShootType, float, float);
    void* GetBulletSprite() const;
    void Shutdown();
    void Init(EWeaponTypes, WeaponProperties_struct*, ISceneNode*, short);
    void StartReloading();
    void Empty();

    unsigned char unknown000[48];
    void* m_shown;
    unsigned char unknown034[588];
    WeaponPropertiesView* m_properties;
    short m_reserve;
    short m_loaded;
    float m_unknown288;
    int m_reloadState;
    float m_shootValue;
    int m_unknown294;
    EWeaponTypes m_type;
    float m_shootValue2;
    unsigned char unknown2a0[16];
    float m_unknown2b0;
    float m_unknown2b4;
    float m_unknown2b8;
    unsigned char unknown2bc[4];
    unsigned char reloading : 1;
    unsigned char flag6 : 1;
    unsigned char triggerFlag : 1;
    unsigned char infiniteAmmo : 1;
    unsigned char flag3 : 1;
    unsigned char flag2 : 1;
    unsigned char flags : 2;
    unsigned char unknown2c1[3];
    float m_unknown2c4;
    float m_unknown2c8;
    void* m_muzzleFlash;
    float m_unknown2d0;
    CWeaponHolderView* m_holder;
    unsigned char m_trigger[4];
    unsigned char unknown2dc[12];
    CWeapon* m_unknown2e8;
    int m_unknown2ec;
    int m_unknown2f0;
    int m_unknown2f4;
    int m_unknown2f8;
};

void CWeapon::Draw(CDrawContext& context) {
    if (m_shown && !m_holder->DrawsWeapon())
        CStaticObject::Draw(context);
}

void CWeapon::BeginUpdate(float elapsed) {
    if (triggerFlag)
        ForceUpdateProximityTriggerStatus((BSGO_Basic*)m_trigger, true);
    CStaticObject::BeginUpdate(elapsed);
}

void CWeapon::DoneReloading() {
    reloading = 0;
    if (infiniteAmmo) {
        m_loaded = m_properties->clipSize;
    } else if (m_loaded + m_reserve < m_properties->clipSize) {
        m_loaded += m_reserve;
        m_reserve = 0;
    } else {
        m_reserve = m_reserve - (m_properties->clipSize - m_loaded);
        m_loaded = m_properties->clipSize;
    }
}

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

void CWeapon::AddAmo(short count) {
    if (m_reserve + m_loaded == 0)
        reloading = 1;
    m_reserve += count;
    m_reserve = m_reserve > m_properties->maxReserve ? m_properties->maxReserve : m_reserve;
}

void CWeapon::Shoot(EWeaponShootType type, float value, float value2) {
    m_reloadState = type;
    m_shootValue = value;
    m_shootValue2 = value2;
}

void* CWeapon::GetBulletSprite() const {
    return g_pBulletFactory->GetBulletSprite(m_properties->bulletSpriteFlag != 0, m_properties->bulletSprite);
}

void CWeapon::Shutdown() {
}

void CWeapon::Init(EWeaponTypes type, WeaponProperties_struct* properties, ISceneNode* holder, short reserve) {
    CStaticObject::Init();
    m_unknown288 = 0.0f;
    m_reloadState = 1;
    flag6 = 0;
    m_unknown2c8 = 4.0f;
    m_holder = (CWeaponHolderView*)holder;
    infiniteAmmo = 0;
    m_properties = (WeaponPropertiesView*)properties;
    m_type = type;
    m_shown = ((WeaponPropertiesView*)properties)->unknown28;
    flag3 = 0;
    flag2 = 0;
    m_unknown2c4 = 0.0f;
    triggerFlag = 0;
    m_unknown2d0 = 1.0f;
    m_reserve = reserve > m_properties->maxReserve ? m_properties->maxReserve : reserve;
    DoneReloading();
    m_unknown2b8 = 0.0f;
    m_unknown2b4 = 0.0f;
    m_unknown2b0 = 0.0f;
    if (m_holder) {
        switch (m_holder->GetWeaponHolderType()) {
        case 2:
        case 13:
        case 23:
            if (m_properties->bulletSpriteFlag)
                m_unknown294 = 4;
            else
                m_unknown294 = 10;
            break;
        case 1:
            if (m_properties->bulletSpriteFlag)
                m_unknown294 = 3;
            else
                m_unknown294 = 9;
            break;
        case 5:
            if (m_properties->bulletSpriteFlag)
                m_unknown294 = 6;
            else
                m_unknown294 = 11;
            break;
        default:
            m_unknown294 = -1;
            break;
        }
    }
    switch (m_type) {
    case 35:
    case 36:
    case 37:
    case 38:
    case 40:
        m_muzzleFlash = 0;
        break;
    default:
        m_muzzleFlash = g_pStaticObjectMuzzleFlashNPC;
        break;
    }
    m_unknown2f0 = 0;
    m_unknown2ec = 0;
    m_unknown2f8 = 0;
    m_unknown2f4 = 0;
    *(int*)m_trigger = 0;
    m_unknown2e8 = this;
}
