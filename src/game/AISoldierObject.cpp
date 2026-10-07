// CAISoldierObject target match lists: the soldier's list of up to a few
// (status, value, weight) matches used when choosing targets, copied from
// the caller or reset to built-in lists by the current target mode. The
// class, enum and structure names come from the mangled symbols; the member
// names and the match fields are inferred. Init and Update follow: Init runs
// the base initialisation, sets the reaction counter (15 times the ratio of
// two members, rounded up), copies a few settings and resets the match list
// for mode 3; Update refreshes the target position when reacting, steps the
// current walk mode (spline, A* path, cover points, arbitrary point), applies
// obstacle avoidance and resets the counter. This is a non-virtual view of
// the class and of a CAIObject base (its virtual table is not reproduced;
// members inferred from offsets); the constants are entries of the file's
// .sdata2 pool. The functions before and after these are not reconstructed
// here.
enum aifilter_target_mode {};

struct aistatus_match {
    int field0;
    int field4;
    int field8;
};

class CAIObject {
public:
    void Init();
    bool UpdateReactionTime();
    void UpdateTargetPosition();
    void WalkNextSplinePathPoint();
    void WalkAStarPath();
    void WalkToCoverPointDirect();
    void WalkToCoverPointIndirect();
    void WalkToCoverPoint();
    void CheckCoverpointUsefulness();
    void WalkToArbitraryPoint();
    void ProcessObstacleAvoidance();

    unsigned char unknown0000[20];
    unsigned char data014;
    unsigned char data015;
    unsigned char unknown0016[174];
    void* m_target;
    unsigned char unknown00c8[236];
    int data1b4;
    int data1b8;
    int data1bc;
    int data1c0;
    int data1c4;
    int data1c8;
    unsigned char unknown01cc[8];
    int data1d4;
    unsigned char unknown01d8[16];
    int data1e8;
    unsigned char unknown01ec[40];
    int data214;
    int m_walkMode;
    unsigned char unknown021c[200];
    bool m_avoidObstacles;
    unsigned char unknown02e5[1155];
    float data768;
    float data76c;
};

class CAISoldierObject : public CAIObject {
public:
    void SetupTargetMatchList(aifilter_target_mode, aistatus_match*, int);
    void ResetTargetMatchList(aifilter_target_mode);
    void Init();
    void Update(float);
    void PreUpdate();

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

void CAISoldierObject::Init() {
    CAIObject::Init();
    data1b4 = 15.0f * (data768 / data76c) + 0.999f;
    data1b8 = data015;
    data1bc = data214;
    data1c0 = 4;
    data1c4 = 1;
    data1c8 = data014;
    data1d4 = 0;
    data1e8 = 0;
    m_targetMode = (aifilter_target_mode)3;
    ResetTargetMatchList((aifilter_target_mode)3);
}

void CAISoldierObject::Update(float) {
    if (UpdateReactionTime() && m_target)
        UpdateTargetPosition();
    switch (m_walkMode) {
    case 1:
        WalkNextSplinePathPoint();
        break;
    case 2:
        WalkAStarPath();
        break;
    case 3:
        WalkToCoverPointDirect();
        break;
    case 4:
        WalkToCoverPointIndirect();
        break;
    case 9:
        WalkToCoverPoint();
        break;
    case 5:
        CheckCoverpointUsefulness();
        break;
    case 8:
        WalkToArbitraryPoint();
        break;
    }
    if (m_avoidObstacles)
        ProcessObstacleAvoidance();
    data1b4 = 15.0f * (data768 / data76c) + 0.999f;
    data1c0 = 4;
    data1c4 = 1;
}

void CAISoldierObject::PreUpdate() {
}
