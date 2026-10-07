// CTankObject::MapWeaponSlot, AddWeapon and GetLookMatrix (a fragment of
// Tank_object.cpp): map a sub-object to a weapon slot; create a slot's weapon
// from its CRC through the weapon factory (the empty-weapon CRC clears the
// slot), choosing the weapon's mode from its properties; and return the look
// sub-object's matrix, or the tank's own raised by one unit. The names come from the mangled
// symbols; the members, the result type and the weapon's fields are inferred
// from offsets (the factory has its symbol's 28-byte size), and the classes are non-virtual views.
class ISceneNode;

// Vector view (inferred): an 8-byte aligned three-float member, which gives
// the doubleword copies; the position getter is an inferred inline helper.
struct VECTOR3VIEW {
    float x;
    float y;
    float z;
} __attribute__((aligned(8)));

class CVector3 {
public:
    VECTOR3VIEW v;
};

class CMatrix {
public:
    CMatrix& operator=(const CMatrix&);
    void SetPos(CVector3);
    CVector3 GetPos() const { return position; }

    CVector3 right;
    CVector3 forward;
    CVector3 up;
    CVector3 position;
};

class CHierObject {
public:
    CHierObject* GetSubObject(int);

    unsigned char unknown000[64];
    CMatrix m_tm;
};

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

class CTankObject : public CHierObject {
public:
    void MapWeaponSlot(int, int);
    bool AddWeapon(int, int);
    void GetLookMatrix(CMatrix&);

    unsigned char unknown080[1940 - 128];
    int m_lookSubObject;
    unsigned char unknown798[4];
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

void CTankObject::GetLookMatrix(CMatrix& matrix) {
    if (m_lookSubObject) {
        matrix = GetSubObject(m_lookSubObject)->m_tm;
    } else {
        CVector3 pos;

        matrix = m_tm;
        pos = matrix.GetPos();
        pos.v.z += 1.0f;
        matrix.SetPos(pos);
    }
}
