// CAITankObject, the first functions of the file: choosing a target (the
// first AI object in the list that is not this tank and is on another team,
// or none) and copying the target match list. CAITankObject, CAIObject,
// aifilter_target_mode and aistatus_match are named by the mangled symbols;
// the members are inferred from offsets and the classes are non-virtual views.
enum aifilter_target_mode {};

struct aistatus_match {
    int unknown00;
    int unknown04;
    int unknown08;
};

class CAIObject {
public:
    void SetTarget(CAIObject*);

    unsigned char unknown00[4];
    CAIObject* m_next;
    unsigned char unknown08[12];
    unsigned char m_team;
};

class CAITankObject : public CAIObject {
public:
    CAIObject* ChooseTarget(CAIObject*, float);
    void SetupTargetMatchList(aifilter_target_mode, aistatus_match*, int);

    unsigned char unknown018[1836];
    aistatus_match m_matches[4];
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
