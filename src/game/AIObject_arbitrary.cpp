// A fragment of AIObject.cpp (0x80049e88): CAIObject::WalkToArbitraryPoint
// (before the next update time it refreshes the arbitrary point for point types
// other than 3 to 10 from the pointer at +196's position, when the flag at
// +1752 is set, or else from the position at +336, writes it through the vector
// pointer, and copies it to the move target; otherwise it sets the next update
// time 0.1 ahead; then it triggers script event 152 on the filter object's
// script object when the XYZ (type 4) or XY distance to the move target less the
// distance at +732 is under 0.5), CAIObject::StartWalkToArbitraryPoint,
// the empty CAIFilterObject::SetMoveTarget it calls virtually (emitted here as a
// weak copy) and CAIObject::SetupArbitraryPointUpdate. CAIFilterObject's
// virtual functions follow its vtable (its script object at +12 is inferred);
// the CAIObject members and the AI filter global's time are inferred from
// offsets in non-virtual views and named by offset where their use is
// unclear, and the switch's explicit cases 0 to 2 are inferred.
struct BSObject;
enum aifilter_target_mode {};
enum EMovePointType {};
struct aistatus_match;
struct BS_STRUCT_Vector_struct;

class CAIFilterRealPosition {
public:
    float GetDistanceSquaredXYZReal(const CAIFilterRealPosition&) const;
    float GetDistanceXYZReal(const CAIFilterRealPosition&) const;
    float GetDistanceXYReal(const CAIFilterRealPosition&) const;

    float x;
    float y;
    float z;
};

class CAIFilterRealVector3;

class CAIFilterObject {
public:
    virtual ~CAIFilterObject();
    virtual void ChooseTarget(float);
    virtual void SetTargetMode(aifilter_target_mode);
    virtual void ResetTargetMatchList(aifilter_target_mode);
    virtual void SetupTargetMatchList(aifilter_target_mode, aistatus_match*, int);
    virtual void SetMoveTarget(const CAIFilterRealPosition&) {}
    virtual void GetLookDirection(CAIFilterRealVector3&, CAIFilterRealVector3&);

    unsigned char unknown04[8];
    BSObject* m_script;
};

class CLinkedListElement;

class CLinkedList {
public:
    void RemoveElement(CLinkedListElement*);

    CLinkedListElement* m_head;
};

class CLinkedListElement {
public:
    unsigned char unknown00[4];
};

class CSearchNode : public CLinkedListElement {
public:
    ~CSearchNode();
};

class CAIFilterGlobal {
public:
    unsigned char unknown00[4];
    float m_time;
    unsigned char unknown08[16];
};

extern CAIFilterGlobal g_aigAIFilterGlobalObject;

void BSObjectTriggerEvent(BSObject*, unsigned short, void*, BSObject*, bool);

class CAIObject {
public:
    void WalkToArbitraryPoint();
    void StartWalkToArbitraryPoint(CAIFilterRealPosition&);
    void SetupArbitraryPointUpdate(EMovePointType, BS_STRUCT_Vector_struct*, float);

    unsigned char unknown000[8];
    CAIFilterObject* m_filterObject;
    unsigned char unknown00c[12];
    CAIFilterRealPosition m_position;
    unsigned char unknown024[44];
    CAIFilterRealPosition m_moveTarget;
    unsigned char unknown05c[104];
    CAIObject* m_unknown0c4;
    unsigned char unknown0c8[136];
    CAIFilterRealPosition m_unknown150;
    unsigned char unknown15c[188];
    int m_unknown218;
    float m_unknown21c;
    float m_unknown220;
    bool m_unknown224;
    bool m_unknown225;
    unsigned char unknown226[50];
    CLinkedList m_searchNodes;
    unsigned char unknown25c[8];
    int m_unknown264;
    int m_unknown268;
    int m_unknown26c;
    unsigned char unknown270[88];
    CAIFilterRealPosition m_arbitraryPoint;
    unsigned char unknown2d4[4];
    EMovePointType m_arbitraryPointType;
    float m_unknown2dc;
    CAIFilterRealPosition* m_unknown2e0;
    unsigned char unknown2e4[1012];
    bool m_unknown6d8;
};

void CAIObject::WalkToArbitraryPoint() {
    if (g_aigAIFilterGlobalObject.m_time < m_unknown21c) {
        switch (m_arbitraryPointType) {
        case 0:
        case 1:
        case 2:
        default:
            if (m_unknown0c4 && m_unknown6d8) {
                float z = m_unknown0c4->m_position.z;
                float y = m_unknown0c4->m_position.y;
                float x = m_unknown0c4->m_position.x;
                m_arbitraryPoint.x = x;
                m_arbitraryPoint.y = y;
                m_arbitraryPoint.z = z;
            } else {
                float z = m_unknown150.z;
                float y = m_unknown150.y;
                float x = m_unknown150.x;
                m_arbitraryPoint.x = x;
                m_arbitraryPoint.y = y;
                m_arbitraryPoint.z = z;
            }
            m_unknown2e0->x = m_arbitraryPoint.x;
            m_unknown2e0->y = m_arbitraryPoint.y;
            m_unknown2e0->z = m_arbitraryPoint.z;
            break;
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
            break;
        }
        float z = m_arbitraryPoint.z;
        float y = m_arbitraryPoint.y;
        float x = m_arbitraryPoint.x;
        m_moveTarget.x = x;
        m_moveTarget.y = y;
        m_moveTarget.z = z;
    } else {
        m_unknown21c = g_aigAIFilterGlobalObject.m_time + 0.1f;
    }
    float distance;
    if (m_arbitraryPointType == 4)
        distance = m_position.GetDistanceXYZReal(m_moveTarget);
    else
        distance = m_position.GetDistanceXYReal(m_moveTarget);
    if (distance - m_unknown2dc < 0.5f)
        BSObjectTriggerEvent(m_filterObject->m_script, 152, 0, 0, false);
}

void CAIObject::StartWalkToArbitraryPoint(CAIFilterRealPosition& point) {
    m_unknown218 = 0;
    m_unknown264 = 0;
    m_unknown268 = 0;
    m_unknown26c = 0;
    float z = m_arbitraryPoint.z;
    float y = m_arbitraryPoint.y;
    float x = m_arbitraryPoint.x;
    m_moveTarget.x = x;
    m_moveTarget.y = y;
    m_moveTarget.z = z;
    m_unknown220 = m_position.GetDistanceSquaredXYZReal(m_moveTarget);
    m_unknown224 = false;
    m_unknown225 = false;
    m_unknown218 = 0;
    CSearchNode* node = (CSearchNode*)m_searchNodes.m_head;
    while (node) {
        m_searchNodes.RemoveElement(node);
        delete node;
        node = (CSearchNode*)m_searchNodes.m_head;
    }
    m_unknown218 = 8;
    float pz = point.z;
    float py = point.y;
    float px = point.x;
    m_arbitraryPoint.x = px;
    m_arbitraryPoint.y = py;
    m_arbitraryPoint.z = pz;
    float tz = m_arbitraryPoint.z;
    float ty = m_arbitraryPoint.y;
    float tx = m_arbitraryPoint.x;
    m_moveTarget.x = tx;
    m_moveTarget.y = ty;
    m_moveTarget.z = tz;
    m_filterObject->SetMoveTarget(m_moveTarget);
}

void CAIObject::SetupArbitraryPointUpdate(EMovePointType type, BS_STRUCT_Vector_struct* vector, float value) {
    m_arbitraryPointType = type;
    m_unknown2dc = value;
    m_unknown2e0 = (CAIFilterRealPosition*)vector;
}
