// A fragment of player.cpp (0x8009ea04): CPlayerObject's falling-damage and
// climbing states (climbing clears another flag bit), script events
// (forwarded to the script object when there is one) and SetScript (stores
// the script and passes it to each of the first 45 weapon objects through
// CAnimObject's virtual SetScript), its spline-path movement (UpdateMovePath
// advances the path time, at half the rate for the sample, clamps it to 0 to
// 1 and ends the path at 1, takes the position from the path, or from the
// velocity without one, derives the velocity from the move, and when there
// is a next path faces along it, using the path's derivative when both paths
// are the same, building the facing matrix from the front, world up and
// their cross products and storing two of its Euler angles; StopPath clears
// the on-path flag; MoveOnPath sets it, stores the path and the next one,
// restarts the time and sets a rate of 1/59.94 (the NTSC field rate) over
// the speed), its motion and camera shakes, then its current-weapon queries.
// ShakeCamera adds random offsets to the two angles: a quarter of the camera
// shake's strength, faded in or out over its time (counting down clears the
// flag at the end), and a quarter of the motion shake's amount, which steps
// towards the strength scaled by the squared speed (velocity at +600, full
// at the speed whose square is 0.0084196). DoMotionShake stores the strength
// and time, restarts the elapsed time and flags the shake while the strength
// is positive; StopCameraShake and StartCameraShake take the time's
// magnitude (counting down or up), store a rate of -1 or 1 per second over
// it (1 for a zero time) and clear or set the shake flag. Their constants
// are entries of the file's .sdata2 pool. While the mounted-weapon flag is
// set the weapon queries read the mounted weapon; otherwise the selected
// slot's weapon object's weapon, with 0 (or null) when no slot is selected:
// the reserve and clip ammo counts and the bullet sprite. GetWeaponByCRC
// returns the weapon object in the slot GetWeaponInfoByCRC finds;
// GetWeaponInfoByCRC maps a weapon-name CRC to a slot and two ids, reporting
// unknown CRCs; GetWeapon, GetCurrentWeapon and GetCurrentPlayerWeapon
// follow. The file name is this project's; the original record is player.cpp
// and CycleWeapon after these is not reconstructed. The classes and
// functions are named by the mangled symbols; CAnimObject is a virtual view
// whose earlier slots are placeholders and the weapon objects are cast to
// it; CAISplinePath's inline expansion by segment and the CVector3 helpers
// are inferred, as is the use of MSL's inline sqrtf; CPlayerObject,
// CPlayerWeaponObject and CWeapon are inferred non-virtual views (members at
// their offsets, names not original) and the result types and the angle
// parameter names are inferred.
class CSprite;

namespace std {
inline float sqrtf(float x)
{
    const double _half = .5;
    const double _three = 3.0;
    volatile float y;
    if (x > 0.0f)
    {
        double guess = __frsqrte((double)x);
        guess = _half*guess*(_three - guess*guess*x);
        guess = _half*guess*(_three - guess*guess*x);
        guess = _half*guess*(_three - guess*guess*x);
        y = (float)(x*guess);
        return y;
    }
    return x;
}
}

class CVector3 {
public:
    CVector3() {}
    CVector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
    CVector3 operator-(const CVector3& o) const { return CVector3(x - o.x, y - o.y, z - o.z); }
    CVector3& operator*=(float f) {
        x *= f;
        y *= f;
        z *= f;
        return *this;
    }
    float LengthSq() const { return x * x + y * y + z * z; }
    void Normalize() {
        float length = std::sqrtf(LengthSq());
        if (length != 0.0f)
            *this *= 1.0f / length;
    }
    void Cross(const CVector3& a, const CVector3& b) {
        x = a.y * b.z - a.z * b.y;
        y = a.z * b.x - a.x * b.z;
        z = a.x * b.y - a.y * b.x;
    }
    void Sub(const CVector3& a, const CVector3& b) {
        x = a.x - b.x;
        y = a.y - b.y;
        z = a.z - b.z;
    }

    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CMatrix {
public:
    CMatrix() {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    void SetRight(CVector3);
    void SetUp(CVector3);
    void SetFront(CVector3);
    void ToEulerXYZ(float&, float&, float&) const;

    float m[4][4];
    static bool s_ClassInit;
} __attribute__((aligned(16)));

class CAISplinePathSegment {
public:
    void Expand(float, CVector3&);
    void ExpandDerivative(float, CVector3&);

    unsigned char unknown00[72];
};

class CAISplinePath {
public:
    void Expand(float t, CVector3& out) {
        float position = t * m_count;
        int index = position;
        position -= index;
        m_segments[index].Expand(position, out);
    }
    void ExpandDerivative(float t, CVector3& out) {
        float position = t * m_count;
        int index = position;
        position -= index;
        m_segments[index].ExpandDerivative(position, out);
    }

    CAISplinePathSegment* m_segments;
    unsigned int m_count;
};


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

class BSObject;

// CAnimObject's virtual functions up to SetScript (+300 in
// __vt__19CPlayerWeaponObject); the earlier slots are placeholders named by
// offset.
class CAnimObject {
public:
    virtual void unknown008();
    virtual void unknown00c();
    virtual void unknown010();
    virtual void unknown014();
    virtual void unknown018();
    virtual void unknown01c();
    virtual void unknown020();
    virtual void unknown024();
    virtual void unknown028();
    virtual void unknown02c();
    virtual void unknown030();
    virtual void unknown034();
    virtual void unknown038();
    virtual void unknown03c();
    virtual void unknown040();
    virtual void unknown044();
    virtual void unknown048();
    virtual void unknown04c();
    virtual void unknown050();
    virtual void unknown054();
    virtual void unknown058();
    virtual void unknown05c();
    virtual void unknown060();
    virtual void unknown064();
    virtual void unknown068();
    virtual void unknown06c();
    virtual void unknown070();
    virtual void unknown074();
    virtual void unknown078();
    virtual void unknown07c();
    virtual void unknown080();
    virtual void unknown084();
    virtual void unknown088();
    virtual void unknown08c();
    virtual void unknown090();
    virtual void unknown094();
    virtual void unknown098();
    virtual void unknown09c();
    virtual void unknown0a0();
    virtual void unknown0a4();
    virtual void unknown0a8();
    virtual void unknown0ac();
    virtual void unknown0b0();
    virtual void unknown0b4();
    virtual void unknown0b8();
    virtual void unknown0bc();
    virtual void unknown0c0();
    virtual void unknown0c4();
    virtual void unknown0c8();
    virtual void unknown0cc();
    virtual void unknown0d0();
    virtual void unknown0d4();
    virtual void unknown0d8();
    virtual void unknown0dc();
    virtual void unknown0e0();
    virtual void unknown0e4();
    virtual void unknown0e8();
    virtual void unknown0ec();
    virtual void unknown0f0();
    virtual void unknown0f4();
    virtual void unknown0f8();
    virtual void unknown0fc();
    virtual void unknown100();
    virtual void unknown104();
    virtual void unknown108();
    virtual void unknown10c();
    virtual void unknown110();
    virtual void unknown114();
    virtual void unknown118();
    virtual void unknown11c();
    virtual void unknown120();
    virtual void unknown124();
    virtual void unknown128();
    virtual void SetScript(BSObject*);
};

void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);

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
    void SetFallingDamageState(bool);
    void SetClimbingState(bool);
    void TriggerScriptEvent(int, void*, bool);
    void SetScript(BSObject*);
    void UpdateMovePath(float);
    void StopPath();
    void MoveOnPath(CAISplinePath*, CAISplinePath*, float);
    void ShakeCamera(float&, float&, float);
    void DoMotionShake(float, float);
    void StopCameraShake(float);
    void StartCameraShake(float, float);

    unsigned char unknown000[592];
    BSObject* m_script;
    unsigned char unknown254[4];
    CVector3 m_velocity;
    unsigned char unknown268[32];
    CVector3 m_position;
    unsigned char unknown298[4];
    float m_angle29c;
    float m_angle2a0;
    unsigned char unknown2a4[240];
    unsigned char unknown394 : 7;
    unsigned char m_climbing : 1;
    unsigned char unknown395a : 2;
    unsigned char m_cameraShaking : 1;
    unsigned char unknown395b : 1;
    unsigned char m_onPath : 1;
    unsigned char unknown395c : 2;
    unsigned char m_flag395 : 1;
    unsigned char m_usingMountedWeapon : 1;
    unsigned char unknown396b : 7;
    unsigned char unknown397a : 2;
    unsigned char m_fallingDamage : 1;
    unsigned char unknown397c : 2;
    unsigned char m_motionShaking : 1;
    unsigned char unknown397b : 2;
    unsigned char unknown398[8];
    float m_cameraShakeStrength;
    float m_cameraShakeRate;
    float m_cameraShakeTime;
    float m_motionShakeStrength;
    float m_motionShakeTime;
    float m_motionShakeElapsed;
    unsigned char unknown3b8[72];
    CAISplinePath* m_path;
    CAISplinePath* m_nextPath;
    float m_pathTime;
    float m_pathRate;
    unsigned char unknown410[76];
    int m_currentWeapon;
    CPlayerWeaponObject* m_weapons[96];
    CWeapon* m_mountedWeapon;
};

void CPlayerObject::SetFallingDamageState(bool state) {
    m_fallingDamage = state;
}

void CPlayerObject::SetClimbingState(bool state) {
    m_climbing = state;
    if (m_climbing)
        m_flag395 = 0;
}

void CPlayerObject::TriggerScriptEvent(int event, void* data, bool flag) {
    if (m_script)
        BSObjectTriggerEvent(m_script, event, data, 0, flag);
}

void CPlayerObject::SetScript(BSObject* script) {
    m_script = script;
    for (int i = 0; i < 45; i++) {
        if (m_weapons[i])
            ((CAnimObject*)m_weapons[i])->SetScript(script);
    }
}

void CPlayerObject::UpdateMovePath(float dt) {
    if (m_onPath) {
        float t = m_pathTime + 0.5f * m_pathRate * dt;
        m_pathTime += m_pathRate * dt;
        if (t < 0.0f)
            t = 0.0f;
        if (t > 1.0f) {
            m_onPath = 0;
            t = 1.0f;
        }
        CVector3 position;
        if (m_path)
            m_path->Expand(t, position);
        else {
            float dx = dt * m_velocity.x;
            float dy = dt * m_velocity.y;
            float dz = dt * m_velocity.z;
            position.x = m_position.x + dx;
            position.y = m_position.y + dy;
            position.z = m_position.z + dz;
        }
        m_velocity.Sub(position, m_position);
        m_velocity *= 1.0f / dt;
        CVector3 front;
        if (m_nextPath && m_nextPath == m_path) {
            m_path->ExpandDerivative(t, front);
            front.Normalize();
        } else if (m_nextPath) {
            CVector3 next;
            m_nextPath->Expand(t, next);
            front.Sub(next, position);
            front.Normalize();
        } else
            return;
        CVector3 up(0.0f, 0.0f, 1.0f);
        CVector3 right;
        right.Cross(front, up);
        right.Normalize();
        up.Cross(right, front);
        up.Normalize();
        CMatrix matrix;
        matrix.SetRight(right);
        matrix.SetUp(up);
        matrix.SetFront(front);
        float unused;
        matrix.ToEulerXYZ(m_angle2a0, unused, m_angle29c);
    }
}

void CPlayerObject::StopPath() {
    m_onPath = 0;
}

void CPlayerObject::MoveOnPath(CAISplinePath* path, CAISplinePath* nextPath, float speed) {
    m_onPath = 1;
    m_path = path;
    m_nextPath = nextPath;
    m_pathTime = 0.0f;
    m_pathRate = (1.0f / 59.94f) * (1.0f / speed);
}

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
        float speed = (m_velocity.x * m_velocity.x + m_velocity.y * m_velocity.y + m_velocity.z * m_velocity.z) / 0.008419609628617764f;
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
