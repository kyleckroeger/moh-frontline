// A fragment of AIObject.cpp (0x8004d0fc): CAIObject::ComputeStatusThreatValue,
// a 0-15 threat level from another object's value at +448, its exponential
// distance class and its capped targeted-by count (+468), the members
// inferred from offsets in a non-virtual view.
class CAIObject {
public:
    int ComputeStatusThreatValue(CAIObject*);
    int ComputeStatusExponentialDistance(CAIObject*);

    unsigned char unknown000[448];
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
