// CAIStatus::MatchStatusList and the empty destructor and constructor, the
// last functions of the file. MatchStatusList checks each match record
// (variable, value, comparison) against the status: GetVarValue (inlined here;
// stored values, or a CAIObject status computed for the static attacker and
// target) compared by equality, inequality, less-than or greater-than. The
// classes and statics are named by the mangled symbols; the record fields,
// the value array and the comparison helper are inferred.
enum aistatus_var {};

struct aistatus_match {
    aistatus_var var;
    int value;
    int comparison;
};

class CAIObject {
public:
    int ComputeStatusIsEnemy(CAIObject*);
    int ComputeStatusLOSValue(CAIObject*);
    int ComputeStatusThreatValue(CAIObject*);
    int ComputeStatusExponentialDistance(CAIObject*);
    int ComputeStatusDistance(CAIObject*);
    int ComputeStatusVisibility(CAIObject*);
};

class CAIStatus {
public:
    CAIStatus();
    ~CAIStatus();
    bool MatchStatusList(aistatus_match*, int);
    int GetVarValue(aistatus_var) const;

    int m_values[14];

    static CAIObject* s_paioAttacker;
    static CAIObject* s_paioTarget;
};

inline int CAIStatus::GetVarValue(aistatus_var var) const {
    int value = 0;
    switch (var) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 8:
    case 13:
        value = m_values[var];
        break;
    case 6:
        value = s_paioAttacker->ComputeStatusVisibility(s_paioTarget);
        break;
    case 7:
        value = s_paioAttacker->ComputeStatusDistance(s_paioTarget);
        break;
    case 9:
        value = s_paioAttacker->ComputeStatusThreatValue(s_paioTarget);
        break;
    case 10:
        value = s_paioAttacker->ComputeStatusLOSValue(s_paioTarget);
        break;
    case 11:
        value = s_paioAttacker->ComputeStatusExponentialDistance(s_paioTarget);
        break;
    case 12:
        value = s_paioAttacker->ComputeStatusIsEnemy(s_paioTarget);
        break;
    }
    return value;
}

static inline bool CompareStatus(int value, int comparison, int target) {
    switch (comparison) {
    case 0:
        if (value == target)
            return true;
        return false;
    case 1:
        if (value != target)
            return true;
        return false;
    case 2:
        if (value < target)
            return true;
        return false;
    case 3:
        if (value > target)
            return true;
        return false;
    }
    return false;
}

bool CAIStatus::MatchStatusList(aistatus_match* match, int count) {
    for (; count > 0; match++, count--) {
        int value;
        int comparison;
        aistatus_var var = match->var;
        comparison = match->comparison;
        value = match->value;
        if (!CompareStatus(GetVarValue(var), comparison, value))
            return false;
    }
    return true;
}

CAIStatus::~CAIStatus() {
}

CAIStatus::CAIStatus() {
}
