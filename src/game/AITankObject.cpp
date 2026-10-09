// CAITankObject, the first functions of the file: choosing a target (the
// first AI object in the list that is not this tank and is on another team,
// or none), copying the target match list (without its count) and resetting
// it to the built-in list for the current target mode, warning for the two
// special modes. CAITankObject, CAIObject,
// aifilter_target_mode and aistatus_match are named by the mangled symbols;
// the members are inferred from offsets and the classes are non-virtual views.
enum aifilter_target_mode {};

struct aistatus_match {
    int unknown00;
    int unknown04;
    int unknown08;
};

void DebugMsg(const char*, ...);

class CAIObject {
public:
    void SetTarget(CAIObject*);
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

    unsigned char unknown00[4];
    CAIObject* m_next;
    unsigned char unknown08[12];
    unsigned char m_team;
    unsigned char data015;
    unsigned char unknown016[174];
    void* m_target;
    unsigned char unknown0c8[236];
    int data1b4;
    int data1b8;
    int data1bc;
    int data1c0;
    int data1c4;
    int data1c8;
    unsigned char unknown01cc[8];
    int data1d4;
    unsigned char unknown01d8[60];
    int data214;
    int m_walkMode;
    unsigned char unknown021c[200];
    bool m_avoidObstacles;
    unsigned char unknown02e5[1107];
};

class CAITankObject : public CAIObject {
public:
    CAIObject* ChooseTarget(CAIObject*, float);
    void SetupTargetMatchList(aifilter_target_mode, aistatus_match*, int);
    void ResetTargetMatchList(aifilter_target_mode);
    void Init();
    void Update(float);
    void PreUpdate();

    float data738;
    float data73c;
    aifilter_target_mode m_targetMode;
    aistatus_match m_matches[4];
    int m_matchCount;
};

CAIObject* CAITankObject::ChooseTarget(CAIObject* candidates, float) {
    for (CAIObject* candidate = candidates; candidate; candidate = candidate->m_next) {
        if (candidate != this && candidate->m_team != m_team) {
            SetTarget(candidate);
            return candidate;
        }
    }
    SetTarget(0);
    return 0;
}

void CAITankObject::SetupTargetMatchList(aifilter_target_mode, aistatus_match* matches, int count) {
    aistatus_match* match = m_matches;

    for (int i = 0; i < count; i++, match++, matches++)
        *match = *matches;
}

void CAITankObject::ResetTargetMatchList(aifilter_target_mode) {
    switch (m_targetMode) {
    case 0:
    case 1:
        DebugMsg("WARNING: ResetTargetMatchList called on special target mode.  This is okay for now, but not if multiple mode override is implemented");
        break;
    case 2:
        m_matches[0].unknown00 = 0;
        m_matches[0].unknown04 = 9;
        m_matches[0].unknown08 = 3;
        m_matches[1].unknown00 = 3;
        m_matches[1].unknown04 = 5;
        m_matches[1].unknown08 = 3;
        m_matches[2].unknown00 = 1;
        m_matches[2].unknown04 = 5;
        m_matches[2].unknown08 = 3;
        m_matches[3].unknown00 = 2;
        m_matches[3].unknown04 = 5;
        m_matches[3].unknown08 = 3;
        m_matchCount = 4;
        break;
    case 3:
        m_matches[0].unknown00 = 0;
        m_matches[0].unknown04 = 9;
        m_matches[0].unknown08 = 3;
        m_matches[1].unknown00 = 3;
        m_matches[1].unknown04 = 5;
        m_matches[1].unknown08 = 3;
        m_matches[2].unknown00 = 1;
        m_matches[2].unknown04 = 5;
        m_matches[2].unknown08 = 3;
        m_matches[3].unknown00 = 2;
        m_matches[3].unknown04 = 5;
        m_matches[3].unknown08 = 3;
        m_matchCount = 4;
        break;
    case 4:
        m_matches[0].unknown00 = 0;
        m_matches[0].unknown04 = 4;
        m_matches[0].unknown08 = 2;
        m_matches[1].unknown00 = 1;
        m_matches[1].unknown04 = 5;
        m_matches[1].unknown08 = 2;
        m_matchCount = 2;
        break;
    case 5:
        m_matches[0].unknown00 = 4;
        m_matches[0].unknown04 = 5;
        m_matches[0].unknown08 = 3;
        m_matches[1].unknown00 = 4;
        m_matches[1].unknown04 = 11;
        m_matches[1].unknown08 = 2;
        m_matches[2].unknown00 = 3;
        m_matches[2].unknown04 = 4;
        m_matches[2].unknown08 = 3;
        m_matches[3].unknown00 = 1;
        m_matches[3].unknown04 = 4;
        m_matches[3].unknown08 = 3;
        m_matchCount = 4;
        break;
    case 6:
        m_matches[0].unknown00 = 0;
        m_matches[0].unknown04 = 4;
        m_matches[0].unknown08 = 2;
        m_matches[1].unknown00 = 1;
        m_matches[1].unknown04 = 5;
        m_matches[1].unknown08 = 2;
        m_matchCount = 2;
        break;
    }
}

void CAITankObject::Init() {
    CAIObject::Init();
    data1b4 = 15.0f * (data738 / data73c) + 0.999f;
    data1b8 = data015;
    data1bc = data214;
    data1c0 = 4;
    data1c4 = 1;
    data1c8 = m_team;
    data1d4 = 0;
    m_targetMode = (aifilter_target_mode)3;
}

void CAITankObject::Update(float) {
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
    data1b4 = 15.0f * (data738 / data73c) + 0.999f;
    data1c0 = 4;
    data1c4 = 1;
}

void CAITankObject::PreUpdate() {
}
