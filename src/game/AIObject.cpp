// CAIObject distance helpers: squared XY, squared XYZ (to another object or to
// the target position) and XYZ distance between AI objects' real positions,
// forwarded to CAIFilterRealPosition. The class names come from the mangled
// symbols; the position (+24) and target position (+240) members are inferred
// from offsets, and CAIObject is a non-virtual view. The rest of the file is
// not part of this unit.
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
    float GetDistanceSquaredXYReal(CAIObject*);
    float GetDistanceSquaredXYZReal();
    float GetDistanceSquaredXYZReal(CAIObject*);
    float GetDistanceXYZReal(CAIObject*);

    unsigned char unknown000[24];
    CAIFilterRealPosition m_position;
    unsigned char unknown024[204];
    CAIFilterRealPosition m_targetPosition;
};

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
