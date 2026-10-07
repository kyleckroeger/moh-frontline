// A fragment of AIObject.cpp (0x8004ce7c): CAIObject::ComputeStatusIsEnemy,
// 15 when the two objects' teams (the byte at +20, inferred) are enemies and 0
// otherwise; teams 1 and 2 oppose each other and team 0 opposes both. The team
// test is an inferred inline helper.
class CAIObject {
public:
    int ComputeStatusIsEnemy(CAIObject*);

    unsigned char unknown000[20];
    unsigned char m_team;
};

static inline bool AreEnemyTeams(unsigned char team, unsigned char other) {
    switch (team) {
    case 1:
        switch (other) {
        case 0:
        case 1:
            return false;
        case 2:
            return true;
        }
        break;
    case 2:
        switch (other) {
        case 0:
        case 2:
            return false;
        case 1:
            return true;
        }
        break;
    case 0:
        switch (other) {
        case 0:
            return false;
        case 1:
        case 2:
            return true;
        }
        break;
    }
    return false;
}

int CAIObject::ComputeStatusIsEnemy(CAIObject* other) {
    return AreEnemyTeams(m_team, other->m_team) ? 15 : 0;
}
