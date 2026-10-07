// A fragment of static_obj.cpp (0x800b031c): CStaticObject's
// CommitPathMotionUpdate (saves the transform's position as the path motion's
// position, copies a speed value and recomputes the path destination and
// speed) and CommitAnimMotionUpdate (while animation motion is active, saves
// the position and sets the rotation at +336 from the transform). CStaticObject,
// CMatrix and CQuaternion are named by the mangled symbols; CStaticObject is
// an inferred non-virtual view, and the CVector3 view and by-value position
// getter are as in static_obj_setpos.cpp. The rest of the file is not part of
// this unit.
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

class CMatrix {
public:
    CVector3 right;
    CVector3 forward;
    CVector3 up;
    CVector3 position;
};

class CQuaternion {
public:
    void SetFromMatrix(const CMatrix&);

    float x;
    float y;
    float z;
    float w;
};

class CStaticObject {
public:
    void CommitPathMotionUpdate();
    void CommitAnimMotionUpdate();
    void UpdatePathMotionDestAndSpeed();

    CVector3 PositionRow() const { return m_tm.position; }

    unsigned char unknown000[64];
    CMatrix m_tm;
    unsigned char unknown080[88];
    float m_speed216;
    unsigned char unknown0dc[68];
    CVector3 m_animPosition;
    unsigned char unknown130[32];
    CQuaternion m_animRotation;
    unsigned char unknown160[16];
    CVector3 m_pathPosition;
    unsigned char unknown180[120];
    float m_pathSpeed;
    unsigned char unknown1fc[40];
    int m_animMotion;
    unsigned char unknown228[4];
    int m_animMotionActive;
};

void CStaticObject::CommitPathMotionUpdate() {
    m_pathPosition = PositionRow();
    m_pathSpeed = m_speed216;
    UpdatePathMotionDestAndSpeed();
}

void CStaticObject::CommitAnimMotionUpdate() {
    if (m_animMotion && m_animMotionActive) {
        m_animPosition = PositionRow();
        m_animRotation.SetFromMatrix(m_tm);
    }
}
