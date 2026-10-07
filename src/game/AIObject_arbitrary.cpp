// A fragment of AIObject.cpp (0x80049fd4): CAIObject::StartWalkToArbitraryPoint,
// the empty CAIFilterObject::SetMoveTarget it calls virtually (emitted here as a
// weak copy) and CAIObject::SetupArbitraryPointUpdate. CAIFilterObject's
// virtual functions follow its vtable; the CAIObject members are inferred from
// offsets in a non-virtual view and named by offset where their use is
// unclear.
enum aifilter_target_mode {};
enum EMovePointType {};
struct aistatus_match;
struct BS_STRUCT_Vector_struct;

class CAIFilterRealPosition {
public:
    float GetDistanceSquaredXYZReal(const CAIFilterRealPosition&) const;

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

class CAIObject {
public:
    void StartWalkToArbitraryPoint(CAIFilterRealPosition&);
    void SetupArbitraryPointUpdate(EMovePointType, BS_STRUCT_Vector_struct*, float);

    unsigned char unknown000[8];
    CAIFilterObject* m_filterObject;
    unsigned char unknown00c[12];
    CAIFilterRealPosition m_position;
    unsigned char unknown024[44];
    CAIFilterRealPosition m_moveTarget;
    unsigned char unknown05c[444];
    int m_unknown218;
    unsigned char unknown21c[4];
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
    BS_STRUCT_Vector_struct* m_unknown2e0;
};

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
    m_unknown2e0 = vector;
}
