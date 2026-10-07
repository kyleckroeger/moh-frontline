// A fragment of player.cpp (0x8009f194): CPlayerObject's motion and camera
// shakes, then its current-weapon queries. ShakeCamera adds random offsets to
// the two angles: a quarter of the camera shake's strength, faded in or out
// over its time (counting down clears the flag at the end), and a quarter of
// the motion shake's amount, which steps towards the strength scaled by the
// squared speed (velocity at +600, full at the speed whose square is
// 0.0084196). DoMotionShake stores the strength
// and time, restarts the elapsed time and flags the shake while the strength
// is positive; StopCameraShake and StartCameraShake take the time's magnitude
// (counting down or up), store a rate of -1 or 1 per second over it (1 for a
// zero time) and clear or set the shake flag. Their constants are entries of
// the file's .sdata2 pool. While the mounted-weapon flag is set the weapon
// queries read the mounted weapon; otherwise the selected slot's weapon
// object's weapon, with 0 (or null) when no slot is selected: the reserve and
// clip ammo counts and the bullet sprite. GetWeaponByCRC returns the weapon
// object in the slot GetWeaponInfoByCRC finds; GetWeaponInfoByCRC maps a
// weapon-name CRC to a slot and two ids, reporting unknown CRCs; GetWeapon,
// GetCurrentWeapon and GetCurrentPlayerWeapon follow. The file name is this
// project's; the original record is player.cpp and CycleWeapon after these is
// not reconstructed. The classes and functions are named by the mangled
// symbols; CPlayerObject, CPlayerWeaponObject and CWeapon are inferred
// non-virtual views (members at their offsets, names not original) and the
// result types and the angle parameter names are inferred.
class CSprite;

void DebugMsg(const char*, ...);

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
    CPlayerWeaponObject* GetWeapon(int) const;
    CWeapon* GetCurrentWeapon() const;
    CPlayerWeaponObject* GetCurrentPlayerWeapon() const;
    void ShakeCamera(float&, float&, float);
    void DoMotionShake(float, float);
    void StopCameraShake(float);
    void StartCameraShake(float, float);

    unsigned char unknown000[600];
    float m_velocityX;
    float m_velocityY;
    float m_velocityZ;
    unsigned char unknown264[305];
    unsigned char unknown395a : 2;
    unsigned char m_cameraShaking : 1;
    unsigned char unknown395b : 5;
    unsigned char m_usingMountedWeapon : 1;
    unsigned char unknown396b : 7;
    unsigned char unknown397a : 5;
    unsigned char m_motionShaking : 1;
    unsigned char unknown397b : 2;
    unsigned char unknown398[8];
    float m_cameraShakeStrength;
    float m_cameraShakeRate;
    float m_cameraShakeTime;
    float m_motionShakeStrength;
    float m_motionShakeTime;
    float m_motionShakeElapsed;
    unsigned char unknown3b8[164];
    int m_currentWeapon;
    CPlayerWeaponObject* m_weapons[96];
    CWeapon* m_mountedWeapon;
};

float MathFunRandomReal(float, float);

void CPlayerObject::ShakeCamera(float& yaw, float& pitch, float dt) {
    if (m_cameraShaking) {
        float time = m_cameraShakeTime;
        float fade = 1.0f - time * m_cameraShakeRate;
        if (time > 0.0f) {
            m_cameraShakeTime = time - dt;
            if (m_cameraShakeTime <= 0.0f)
                m_cameraShakeTime = 0.0f;
        } else if (time < 0.0f) {
            m_cameraShakeTime = time + dt;
            fade = 1.0f - fade;
            if (m_cameraShakeTime >= 0.0f) {
                m_cameraShakeTime = 0.0f;
                m_cameraShaking = 0;
            }
        }
        float range = 0.25f * m_cameraShakeStrength * fade;
        pitch += MathFunRandomReal(-range, range);
        yaw += MathFunRandomReal(-range, range);
    }
    if (m_motionShaking) {
        float step = m_motionShakeStrength / m_motionShakeTime;
        float speed = (m_velocityX * m_velocityX + m_velocityY * m_velocityY + m_velocityZ * m_velocityZ) / 0.008419609628617764f;
        if (speed > 1.0f)
            speed = 1.0f;
        if (speed * m_motionShakeStrength >= m_motionShakeElapsed)
            m_motionShakeElapsed += step;
        else
            m_motionShakeElapsed -= step;
        if (m_motionShakeElapsed > m_motionShakeStrength)
            m_motionShakeElapsed = m_motionShakeStrength;
        else if (m_motionShakeElapsed < 0.0f)
            m_motionShakeElapsed = 0.0f;
        float range = 0.25f * m_motionShakeElapsed;
        pitch += MathFunRandomReal(-range, range);
        yaw += MathFunRandomReal(-range, range);
    }
}

void CPlayerObject::DoMotionShake(float strength, float time) {
    m_motionShakeStrength = strength;
    m_motionShakeTime = time;
    m_motionShakeElapsed = 0.0f;
    if (strength > 0.0f)
        m_motionShaking = 1;
    else
        m_motionShaking = 0;
}

void CPlayerObject::StopCameraShake(float time) {
    if (time < 0.0f)
        time = -time;
    m_cameraShakeTime = -time;
    if (time == 0.0f) {
        m_cameraShaking = 0;
        time = 1.0f;
    }
    m_cameraShakeRate = -1.0f / time;
}

void CPlayerObject::StartCameraShake(float strength, float time) {
    if (time < 0.0f)
        time = -time;
    m_cameraShakeStrength = strength;
    m_cameraShakeTime = time;
    if (time == 0.0f)
        time = 1.0f;
    m_cameraShakeRate = 1.0f / time;
    m_cameraShaking = 1;
}

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

// The CRCs are of weapon names; the two values written for each are ids
// whose meaning is not established (-1 for none).
int CPlayerObject::GetWeaponInfoByCRC(int crc, int* first, int* second) {
    int unused;
    if (!first)
        first = &unused;
    if (!second)
        second = &unused;
    switch (crc) {
    case -1896116718:
    case -1021255877:
    case 1553313914:
        *first = 348;
        *second = 534;
        return 14;
    case -864943398:
    case -665576871:
    case 958924262:
        *first = 349;
        *second = 535;
        return 20;
    case -1355122713:
    case -871513808:
    case -195145515:
        *first = 350;
        *second = 270;
        return 29;
    case -1736123817:
    case -1096117951:
        *first = 351;
        *second = 533;
        return 7;
    case -1379654328:
    case 921360410:
    case 1208277298:
        *first = 352;
        *second = 538;
        return 16;
    case -1738841586:
    case -154155496:
    case 1851280536:
        *first = 353;
        *second = 539;
        return 15;
    case -1909284242:
    case -966596322:
    case 209948952:
        *first = 354;
        *second = 536;
        return 21;
    case -1659995028:
    case 1141236603:
        *first = 355;
        *second = 540;
        return 25;
    case 608597343:
    case 1796663563:
        *first = 356;
        *second = 541;
        return 24;
    case -1878322165:
    case -1572858027:
        *first = 362;
        *second = 272;
        return 30;
    case -1520575955:
    case 503590668:
        *first = -1;
        *second = 271;
        return 33;
    case -1464244323:
    case -86564309:
        *first = 357;
        *second = 542;
        return 5;
    case -2029810030:
    case -2019279670:
    case 917370864:
        *first = 359;
        *second = 543;
        return 8;
    case -284586767:
    case 1193826648:
        *first = 360;
        *second = -1;
        return 36;
    case 704609950:
    case 799037206:
        *first = 361;
        *second = -1;
        return 35;
    case -772383655:
    case 333140481:
        *first = -1;
        *second = -1;
        return 37;
    case 288002954:
    case 1333379419:
        *first = 363;
        *second = 544;
        return 32;
    case -2090384249:
    case 441020331:
    case 1482684816:
        *first = -1;
        *second = 545;
        return 34;
    case -2001962608:
    case -1048131012:
    case 1920496499:
        *first = 358;
        *second = 537;
        return 6;
    default:
        DebugMsg("Unrecognized player weapon CRC: %d\n", crc);
        *first = -1;
        *second = -1;
        return 14;
    }
}

CPlayerWeaponObject* CPlayerObject::GetWeapon(int slot) const {
    return m_weapons[slot];
}

CWeapon* CPlayerObject::GetCurrentWeapon() const {
    if (m_usingMountedWeapon)
        return m_mountedWeapon;
    if (m_currentWeapon >= 0)
        return m_weapons[m_currentWeapon]->m_weapon;
    return 0;
}

CPlayerWeaponObject* CPlayerObject::GetCurrentPlayerWeapon() const {
    if (m_usingMountedWeapon)
        return 0;
    if (m_currentWeapon >= 0)
        return m_weapons[m_currentWeapon];
    return 0;
}
