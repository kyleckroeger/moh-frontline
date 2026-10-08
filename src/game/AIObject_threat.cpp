// A fragment of AIObject.cpp (0x8004cf48): CAIObject::ComputeStatusLOSValue,
// 15 when another object is in line of sight, else 0 (the vision range at +520
// halved when the other's flag at +1840 is set and scaled by 0.75 when it is
// crouching, none when bit 8 of +432 is set, 50 with a warning when at most
// 0.001; within range, a vision line from the filter object's look position to
// the other's position raised by each of two heights, standing or crouched,
// not blocked by the environment); CAIObject::ComputeStatusThreatValue,
// a 0-15 threat level from another object's value at +448, its exponential
// distance class and its capped targeted-by count (+468);
// ComputeStatusExponentialDistance, the class 0-15 of the XYZ distance
// between the squares 1, 4, ..., 225 (a balanced comparison tree);
// ComputeStatusDistance, a tenth of the XY distance capped at 15; and
// ComputeStatusVisibility (always 0). CAIObject's and CAIFilterObject's
// virtual functions follow their vtables; the members, the line-of-sight helper and the height tables are
// inferred from offsets and the code; the thresholds are entries of the file's
// .sdata2 pool.
enum aifilter_target_mode {};
struct aistatus_match;

class CAIFilterRealPosition {
public:
    float GetDistanceXYZReal(const CAIFilterRealPosition&) const;
    float GetDistanceXYReal(const CAIFilterRealPosition&) const;

    float x;
    float y;
    float z;
    unsigned char unknown0c[4];
};

class CAIFilterRealVector3 : public CAIFilterRealPosition {
};

class CAIFilterObject {
public:
    virtual ~CAIFilterObject();
    virtual void ChooseTarget(float);
    virtual void SetTargetMode(aifilter_target_mode);
    virtual void ResetTargetMatchList(aifilter_target_mode);
    virtual void SetupTargetMatchList(aifilter_target_mode, aistatus_match*, int);
    virtual void SetMoveTarget(const CAIFilterRealPosition&);
    virtual void GetLookDirection(CAIFilterRealVector3&, CAIFilterRealVector3&);
};

class CAIFilterGlobal {
public:
    static bool CheckVisionLineToEnvironment(const CAIFilterRealPosition&, const CAIFilterRealPosition&, CAIFilterObject*);
};

extern "C" int printf(const char*, ...);

class CAIObject {
public:
    virtual ~CAIObject();
    virtual void PreUpdate();
    virtual void Update(float);
    virtual bool IsCrouching() const;
    virtual bool IsPlayer() const;
    int ComputeStatusLOSValue(CAIObject*);
    bool HasLineOfSight(CAIObject*);
    int ComputeStatusThreatValue(CAIObject*);
    int ComputeStatusExponentialDistance(CAIObject*);
    int ComputeStatusDistance(CAIObject*);
    int ComputeStatusVisibility(CAIObject*);

    unsigned char unknown004[4];
    CAIFilterObject* m_filterObject;
    unsigned char unknown00c[12];
    CAIFilterRealPosition m_position;
    unsigned char unknown028[392];
    unsigned int m_flags1b0;
    unsigned char unknown1b4[12];
    int m_unknown1c0;
    unsigned char unknown1c4[16];
    int m_unknown1d4;
    unsigned char unknown1d8[48];
    float m_unknown208;
    unsigned char unknown20c[1316];
    bool m_unknown730;
};

inline bool CAIObject::HasLineOfSight(CAIObject* other) {
    float range = m_unknown208;
    if (other->m_unknown730)
        range *= 0.5f;
    if (other->IsCrouching())
        range *= 0.75f;
    bool crouching = other->IsCrouching();
    if ((m_flags1b0 & 8) == 8)
        return false;
    if (range <= 0.001f) {
        printf("Warning: IOIVD - Vision distance is zero, changing to default\n");
        range = 50.0f;
    }
    if (!(m_position.GetDistanceXYZReal(other->m_position) <= range))
        return false;
    CAIFilterRealVector3 direction;
    CAIFilterRealVector3 eye;
    m_filterObject->GetLookDirection(direction, eye);
    float standing[2] = {0.8f, 1.2f};
    float crouched[2] = {0.8f, 0.1f};
    float* heights = standing;
    int count = 2;
    if (crouching) {
        heights = crouched;
        count = 2;
    }
    for (int i = 0; i < count; i++) {
        CAIFilterRealPosition target;
        float y;
        float z = other->m_position.z;
        y = other->m_position.y;
        float x = other->m_position.x;
        target.x = x;
        target.y = y;
        target.z = z;
        target.z = target.z + heights[i];
        if (!CAIFilterGlobal::CheckVisionLineToEnvironment(eye, target, m_filterObject))
            return true;
    }
    return false;
}

int CAIObject::ComputeStatusLOSValue(CAIObject* other) {
    return HasLineOfSight(other) ? 15 : 0;
}

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
