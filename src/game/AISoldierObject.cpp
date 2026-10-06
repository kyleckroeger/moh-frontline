// CAISoldierObject target match lists: the soldier's list of up to a few
// (status, value, weight) matches used when choosing targets, copied from
// the caller or reset to built-in lists by the current target mode. The
// class, enum and structure names come from the mangled symbols; the member
// names and the match fields are inferred. This is a non-virtual view of the
// class (its virtual table is not reproduced). The functions before and after
// these are not reconstructed here.
enum aifilter_target_mode {};

struct aistatus_match {
    int field0;
    int field4;
    int field8;
};

class CAISoldierObject {
public:
    void SetupTargetMatchList(aifilter_target_mode, aistatus_match*, int);
    void ResetTargetMatchList(aifilter_target_mode);

    unsigned char unknown0000[1904];
    aifilter_target_mode m_targetMode;
    aistatus_match m_matches[4];
    int m_matchCount;
};

void CAISoldierObject::SetupTargetMatchList(aifilter_target_mode, aistatus_match* matches, int count) {
    aistatus_match* match = m_matches;

    m_matchCount = count;
    for (int i = 0; i < count; i++, match++, matches++)
        *match = *matches;
}

void CAISoldierObject::ResetTargetMatchList(aifilter_target_mode) {
    switch (m_targetMode) {
    case 2:
        m_matches[0].field0 = 0;
        m_matches[0].field4 = 9;
        m_matches[0].field8 = 3;
        m_matches[1].field0 = 3;
        m_matches[1].field4 = 5;
        m_matches[1].field8 = 3;
        m_matches[2].field0 = 1;
        m_matches[2].field4 = 5;
        m_matches[2].field8 = 3;
        m_matches[3].field0 = 2;
        m_matches[3].field4 = 5;
        m_matches[3].field8 = 3;
        m_matchCount = 4;
        break;
    case 3:
        m_matches[0].field0 = 0;
        m_matches[0].field4 = 9;
        m_matches[0].field8 = 3;
        m_matches[1].field0 = 3;
        m_matches[1].field4 = 5;
        m_matches[1].field8 = 3;
        m_matches[2].field0 = 1;
        m_matches[2].field4 = 5;
        m_matches[2].field8 = 3;
        m_matches[3].field0 = 2;
        m_matches[3].field4 = 5;
        m_matches[3].field8 = 3;
        m_matchCount = 4;
        break;
    case 4:
        m_matches[0].field0 = 0;
        m_matches[0].field4 = 4;
        m_matches[0].field8 = 2;
        m_matches[1].field0 = 1;
        m_matches[1].field4 = 5;
        m_matches[1].field8 = 2;
        m_matchCount = 2;
        break;
    case 5:
        m_matches[0].field0 = 4;
        m_matches[0].field4 = 5;
        m_matches[0].field8 = 3;
        m_matches[1].field0 = 4;
        m_matches[1].field4 = 11;
        m_matches[1].field8 = 2;
        m_matches[2].field0 = 3;
        m_matches[2].field4 = 4;
        m_matches[2].field8 = 3;
        m_matches[3].field0 = 1;
        m_matches[3].field4 = 4;
        m_matches[3].field8 = 3;
        m_matchCount = 4;
        break;
    case 6:
        m_matches[0].field0 = 0;
        m_matches[0].field4 = 4;
        m_matches[0].field8 = 2;
        m_matches[1].field0 = 1;
        m_matches[1].field4 = 5;
        m_matches[1].field8 = 2;
        m_matchCount = 2;
        break;
    }
}
