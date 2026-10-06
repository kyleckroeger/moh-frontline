// CWeaponFactory's destructor (nothing to release) and constructor (empty
// lists and a cleared flag). CWeaponFactory is named by the mangled symbols;
// the members are inferred from offsets. The rest of the file is not part of
// this unit.
class CWeaponFactory {
public:
    CWeaponFactory();
    ~CWeaponFactory();

    void* m_field00;
    void* m_field04;
    void* m_field08;
    void* m_field0c;
    void* m_field10;
    bool m_field14;
};

CWeaponFactory::~CWeaponFactory() {
}

CWeaponFactory::CWeaponFactory() {
    m_field00 = 0;
    m_field08 = 0;
    m_field0c = 0;
    m_field10 = 0;
    m_field14 = false;
}
