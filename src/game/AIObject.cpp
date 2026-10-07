// CAIObject::SetTarget and the distance helpers: squared XY, squared XYZ (to
// another object or to the target position) and XYZ distance between AI
// objects' real positions, forwarded to CAIFilterRealPosition. The class names
// come from the mangled symbols; the position (+24), target (+196) and target
// position (+240) members are inferred from offsets, the other members SetTarget
// uses are named by offset, and CAIObject is a non-virtual view. The rest of
// the file is not part of this unit.
class CAIFilterRealPosition {
public:
    float GetDistanceSquaredXYReal(const CAIFilterRealPosition&) const;
    float GetDistanceSquaredXYZReal(const CAIFilterRealPosition&) const;
    float GetDistanceXYZReal(const CAIFilterRealPosition&) const;

    float x;
    float y;
    float z;
};

class CAIObject {
public:
    CAIObject* SetTarget(CAIObject*);
    void CalculateTargetPosition();
    float GetDistanceSquaredXYReal(CAIObject*);
    float GetDistanceSquaredXYZReal();
    float GetDistanceSquaredXYZReal(CAIObject*);
    float GetDistanceXYZReal(CAIObject*);

    unsigned char unknown000[24];
    CAIFilterRealPosition m_position;
    unsigned char unknown024[156];
    bool m_unknown0c0;
    unsigned char unknown0c1[3];
    CAIObject* m_target;
    unsigned char unknown0c8[40];
    CAIFilterRealPosition m_targetPosition;
    unsigned char unknown0fc[84];
    CAIFilterRealPosition m_unknown150;
    unsigned char unknown15c[120];
    int m_unknown1d4;
    unsigned char unknown1d8[20];
    int m_unknown1ec;
    unsigned char unknown1f0[40];
    int m_unknown218;
    unsigned char unknown21c[188];
    int m_unknown2d8;
    unsigned char unknown2dc[1020];
    bool m_unknown6d8;
    unsigned char unknown6d9[88];
    bool m_unknown731;
};

// Inferred: the count at +492 and its value capped at 15 at +468, kept on
// the targeted object.
static inline void AdjustTargetedCount(CAIObject* object, int delta) {
    object->m_unknown1ec += delta;
    if (object->m_unknown1ec <= 15)
        object->m_unknown1d4 = object->m_unknown1ec;
    else
        object->m_unknown1d4 = 15;
}

CAIObject* CAIObject::SetTarget(CAIObject* target) {
    m_unknown0c0 = false;
    if (target == m_target)
        return m_target;
    m_unknown6d8 = false;
    CAIObject* old = m_target;
    m_target = target;
    if (target) {
        float z = target->m_position.z;
        float y = target->m_position.y;
        float x = target->m_position.x;
        m_unknown150.x = x;
        m_unknown150.y = y;
        m_unknown150.z = z;
        m_unknown6d8 = true;
        m_unknown731 = true;
        CalculateTargetPosition();
    } else if (m_unknown218 == 8 || m_unknown218 == 2) {
        if (m_unknown2d8 == 1 || m_unknown2d8 == 2)
            m_unknown2d8 = 3;
    }
    if (target)
        AdjustTargetedCount(target, 1);
    if (old)
        AdjustTargetedCount(old, -1);
    return old;
}

float CAIObject::GetDistanceSquaredXYReal(CAIObject* other) {
    return m_position.GetDistanceSquaredXYReal(other->m_position);
}

float CAIObject::GetDistanceSquaredXYZReal() {
    return m_position.GetDistanceSquaredXYZReal(m_targetPosition);
}

float CAIObject::GetDistanceSquaredXYZReal(CAIObject* other) {
    return m_position.GetDistanceSquaredXYZReal(other->m_position);
}

float CAIObject::GetDistanceXYZReal(CAIObject* other) {
    return m_position.GetDistanceXYZReal(other->m_position);
}
