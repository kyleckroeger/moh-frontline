// A fragment of AIObject.cpp (0x8004d0fc): CAIObject::ComputeStatusThreatValue,
// a 0-15 threat level from another object's value at +448, its exponential
// distance class and its capped targeted-by count (+468);
// ComputeStatusExponentialDistance, the class 0-15 of the XYZ distance
// between the squares 1, 4, ..., 225 (a balanced comparison tree);
// ComputeStatusDistance, a tenth of the XY distance capped at 15; and
// ComputeStatusVisibility (always 0). The members are inferred from offsets
// in a non-virtual view; the thresholds are entries of the file's .sdata2
// pool.
class CAIFilterRealPosition {
public:
    float GetDistanceXYZReal(const CAIFilterRealPosition&) const;
    float GetDistanceXYReal(const CAIFilterRealPosition&) const;

    unsigned char unknown00[16];
};

class CAIObject {
public:
    int ComputeStatusThreatValue(CAIObject*);
    int ComputeStatusExponentialDistance(CAIObject*);
    int ComputeStatusDistance(CAIObject*);
    int ComputeStatusVisibility(CAIObject*);

    unsigned char unknown000[24];
    CAIFilterRealPosition m_position;
    unsigned char unknown028[408];
    int m_unknown1c0;
    unsigned char unknown1c4[16];
    int m_unknown1d4;
};

int CAIObject::ComputeStatusThreatValue(CAIObject* other) {
    int value = other->m_unknown1c0;
    int threat = value + (15 - ComputeStatusExponentialDistance(other)) * 2;
    threat /= other->m_unknown1d4 + 3;
    if (threat < 0)
        threat = 0;
    else if (threat > 15)
        threat = 15;
    return threat;
}

int CAIObject::ComputeStatusExponentialDistance(CAIObject* other) {
    float distance = m_position.GetDistanceXYZReal(other->m_position);
    if (distance < 64.0f) {
        if (distance < 16.0f) {
            if (distance < 4.0f) {
                if (distance < 1.0f)
                    return 0;
                return 1;
            }
            if (distance < 9.0f)
                return 2;
            return 3;
        }
        if (distance < 36.0f) {
            if (distance < 25.0f)
                return 4;
            return 5;
        }
        if (distance < 49.0f)
            return 6;
        return 7;
    }
    if (distance < 144.0f) {
        if (distance < 100.0f) {
            if (distance < 81.0f)
                return 8;
            return 9;
        }
        if (distance < 121.0f)
            return 10;
        return 11;
    }
    if (distance < 196.0f) {
        if (distance < 169.0f)
            return 12;
        return 13;
    }
    if (distance < 225.0f)
        return 14;
    return 15;
}

int CAIObject::ComputeStatusDistance(CAIObject* other) {
    float distance = 0.1f * m_position.GetDistanceXYReal(other->m_position);
    if (distance >= 15.0f)
        return 15;
    return distance;
}

int CAIObject::ComputeStatusVisibility(CAIObject*) {
    return 0;
}
