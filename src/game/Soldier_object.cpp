// CSoldierObject, the functions at the start of the file: the AI doodad
// accessor and identity casts, script destruction, head tracking and fading.
// The class names come from the mangled symbols. This is an inferred, non-virtual view: only the
// members these functions touch are declared, at their offsets (their names
// are not original), and the virtual table is not reproduced. The accessor
// and casts are inline in the original (weak symbols), so they are defined
// __declspec(weak). SetBulletEmitter follows (12 lines off, register choice in
// the emitter-name search; draft in scratch/lib/Soldier_object_wip.cpp), so
// this unit stops here.
class CAIDoodad;

// The object at BSObject+12; only its first virtual slot is called here, and
// its name is unknown (as in bsmachin.cpp).
class BSObjectUserView {
    unsigned char unknown00[12];

public:
    virtual void unknownVirtual0();
};

struct BSObject {
    unsigned char unknown00[12];
    BSObjectUserView* user;
};

class CSoldierObject {
public:
    const CAIDoodad* GetAIDoodad() const;
    const CSoldierObject* AsSoldierObject() const;
    CSoldierObject* AsSoldierObject();
    void Destroy();
    void SetHeadTrans(bool);
    void SetHeadTrackPlayer(bool, int);
    void FadeOut(float);

    unsigned char unknown0000[9104];
    BSObject* m_script;
    unsigned char unknown2394[52];
    float m_headTurn0;
    float m_headTurn1;
    float m_headTurn2;
    unsigned char unknown23d4[3340];
    unsigned char m_aiDoodad[4];
    unsigned char unknown30e4[4812];
    bool m_fading;
    unsigned char unknown43b1[3];
    float m_fadeTime;
    float m_fadeDuration;
    unsigned char unknown43bc[4];
    bool m_fadeFlag;
    bool m_headTrackPlayer;
    unsigned char unknown43c2[2];
    int m_headTrackTarget;
    int m_headTrackChanged;
    unsigned char unknown43cc[204];
    signed char m_headTransState;
};

__declspec(weak) const CAIDoodad* CSoldierObject::GetAIDoodad() const {
    return (const CAIDoodad*)m_aiDoodad;
}

__declspec(weak) const CSoldierObject* CSoldierObject::AsSoldierObject() const {
    return this;
}

__declspec(weak) CSoldierObject* CSoldierObject::AsSoldierObject() {
    return this;
}

void CSoldierObject::Destroy() {
    if (m_script && m_script->user)
        m_script->user->unknownVirtual0();
}

void CSoldierObject::SetHeadTrans(bool trans) {
    if (m_headTransState <= 1)
        m_headTransState = trans;
    else if (m_headTransState == 2 && !trans)
        m_headTransState = 3;
}

void CSoldierObject::SetHeadTrackPlayer(bool track, int target) {
    if (track != m_headTrackPlayer) {
        m_headTrackPlayer = track;
        m_headTrackTarget = target;
        m_headTrackChanged = 1;
    }
}

void CSoldierObject::FadeOut(float duration) {
    m_fading = true;
    m_fadeTime = 0.0f;
    m_fadeDuration = duration;
    m_headTurn0 = m_headTurn1 = m_headTurn2 = 0.0f;
    m_fadeFlag = false;
}
