// A fragment of AIObject.cpp (0x80049dd4): CAIObject::ProcessObstacleAvoidance
// and EngageObstacleAvoidance, driving the object's CObstacleAvoidance (+744)
// from a position at +80. The members are inferred from offsets in a
// non-virtual view; the position copied to +1616, the objective position at
// +1672, the engaged flag at +740 and the squared distance at +1660 are named
// by offset.
class CAIFilterRealPosition {
public:
    float x;
    float y;
    float z;
};

class CObstacleAvoidance {
public:
    void RunEnvironmentCollisionChecks();
    void DetermineNewObjectivePosition(CAIFilterRealPosition&);
    void PrepareForAvoidance(CAIFilterRealPosition&);
};

class CAIObject {
public:
    void ProcessObstacleAvoidance();
    void EngageObstacleAvoidance(bool, float, float, float);

    unsigned char unknown000[80];
    CAIFilterRealPosition m_unknown050;
    unsigned char unknown05c[648];
    bool m_unknown2e4;
    unsigned char unknown2e5[3];
    CObstacleAvoidance m_obstacleAvoidance;
    unsigned char unknown2e9[871];
    CAIFilterRealPosition m_unknown650;
    unsigned char unknown65c[32];
    float m_unknown67c;
    unsigned char unknown680[8];
    CAIFilterRealPosition m_unknown688;
};

void CAIObject::ProcessObstacleAvoidance() {
    float z = m_unknown050.z;
    float y = m_unknown050.y;
    float x = m_unknown050.x;
    m_unknown650.x = x;
    m_unknown650.y = y;
    m_unknown650.z = z;
    m_obstacleAvoidance.RunEnvironmentCollisionChecks();
    m_obstacleAvoidance.DetermineNewObjectivePosition(m_unknown688);
}

void CAIObject::EngageObstacleAvoidance(bool engage, float, float, float distance) {
    m_unknown2e4 = engage;
    if (m_unknown2e4) {
        m_obstacleAvoidance.PrepareForAvoidance(m_unknown050);
        m_unknown67c = distance * distance;
    }
}
