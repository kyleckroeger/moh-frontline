// A fragment of AIObject.cpp (0x8004eee0): the CAIObject destructor (the
// target dropped as SetTarget(0) does, the object removed from the global
// list, its search nodes unlinked and deleted, then its member objects'
// destructors), the constructor (members cleared or set to their defaults:
// the parameters 0.4, 5 degrees in radians, 2, 2, 20, 30, 20; the status, spline-path
// traversal, search-node list and obstacle-avoidance members constructed; a
// pointer to itself at +1648) and the empty CAIObjectParameters destructor
// (the parameters are the member at +496; their defaults come from an
// inferred inline constructor). The empty destructor, after the others in
// the image, is inlined into CAIObject's destructor but called from the
// constructor's clean-up, so this unit uses deferred inlining with the
// functions in reverse image order.
// The file name is this project's; the original record is AIObject.cpp. The
// destructor is CAIObject's key function, so this unit is compiled with RTTI
// on and holds the class's virtual table and type information; the inline
// IsCrouching and IsPlayer are emitted elsewhere. The classes and functions
// are named by the mangled symbols; the members are inferred from offsets
// and named by offset, the zeroing vector constructor and the target-count
// helper are inferred, the member classes are views of the size their
// offsets allow, and the result types of IsCrouching and IsPlayer are not
// known. The constants are entries of the file's .sdata2 pool.
class CLinkedListElement {
public:
    unsigned char unknown00[4];
};

class CLinkedList {
public:
    void RemoveElement(CLinkedListElement*);

    CLinkedListElement* m_head;
};

class CSearchNode : public CLinkedListElement {
public:
    ~CSearchNode();
};

class CSearchNodeList : public CLinkedList {
public:
    CSearchNodeList();
    ~CSearchNodeList();

    unsigned char unknown04[52];
};

class CAIStatus {
public:
    CAIStatus();
    ~CAIStatus();

    int m_values[14];
};

class CAISplinePathTraversal {
public:
    CAISplinePathTraversal();
    ~CAISplinePathTraversal();

    unsigned char unknown00[40];
};

class CObstacleAvoidance {
public:
    CObstacleAvoidance();
    ~CObstacleAvoidance();

    unsigned char unknown000[904];
};

/* Inferred: the AI tuning parameters at +496 and their defaults. */
class CAIObjectParameters {
public:
    CAIObjectParameters()
        : m_unknown00(0.4f), m_unknown04(0.08726646f), m_unknown08(2.0f), m_unknown0c(2.0f), m_unknown14(20.0f),
          m_unknown18(30.0f), m_unknown20(20.0f), m_unknown28(0), m_unknown2c(0.0f), m_unknown38(0) {}
    ~CAIObjectParameters();

    float m_unknown00;
    float m_unknown04;
    float m_unknown08;
    float m_unknown0c;
    unsigned char unknown10[4];
    float m_unknown14;
    float m_unknown18;
    unsigned char unknown1c[4];
    float m_unknown20;
    unsigned char unknown24[4];
    int m_unknown28;
    float m_unknown2c;
    unsigned char unknown30[8];
    int m_unknown38;
    unsigned char unknown3c[4];
};

/* Inferred: a vector whose default constructor clears it. */
class CAIObjectVectorView {
public:
    CAIObjectVectorView() : x(0.0f), y(0.0f), z(0.0f) {}

    float x;
    float y;
    float z;
};

class CAIObject {
public:
    CAIObject();
    virtual ~CAIObject();
    virtual void PreUpdate();
    virtual void Update(float);
    virtual bool IsCrouching() const; /* result type not known */
    virtual bool IsPlayer() const; /* result type not known */
    void RemoveFromGlobalList();

    int m_unknown004;
    unsigned char unknown008[4];
    float m_unknown00c;
    float m_unknown010;
    unsigned char unknown014[172];
    bool m_unknown0c0;
    unsigned char unknown0c1[3];
    CAIObject* m_target;
    unsigned char unknown0c8[160];
    CAIObjectVectorView m_unknown168;
    unsigned char unknown174[4];
    CAIObjectVectorView m_unknown178;
    unsigned char unknown184[36];
    int m_unknown1a8;
    int m_unknown1ac;
    int m_unknown1b0;
    CAIStatus m_status;
    int m_unknown1ec;
    CAIObjectParameters m_parameters;
    CAISplinePathTraversal m_spline;
    CSearchNodeList m_searchNodes;
    int m_unknown290;
    unsigned char unknown294[48];
    float m_unknown2c4;
    unsigned char unknown2c8[16];
    int m_unknown2d8;
    unsigned char unknown2dc[8];
    bool m_unknown2e4;
    unsigned char unknown2e5[3];
    CObstacleAvoidance m_obstacle;
    CAIObject* m_unknown670;
    unsigned char unknown674[36];
    int m_unknown698;
    int m_unknown69c;
    unsigned char unknown6a0[56];
    bool m_unknown6d8;
    unsigned char unknown6d9[87];
    bool m_unknown730;
    bool m_unknown731;
};


// Inferred: the count at +492 and its value capped at 15 at +468, kept on
// the targeted object (as in AIObject.cpp).
static inline void AdjustTargetedCount(CAIObject* object, int delta) {
    object->m_unknown1ec += delta;
    if (object->m_unknown1ec <= 15)
        object->m_status.m_values[8] = object->m_unknown1ec;
    else
        object->m_status.m_values[8] = 15;
}

CAIObjectParameters::~CAIObjectParameters() {
}

CAIObject::CAIObject()
    : m_unknown004(0), m_unknown00c(0.0f), m_unknown010(0.0f), m_unknown0c0(false), m_target(0),
      m_unknown1a8(0), m_unknown1ac(0), m_unknown1b0(0), m_unknown1ec(0), m_unknown290(0), m_unknown2c4(0.0f), m_unknown2e4(false), m_unknown698(0), m_unknown69c(0),
      m_unknown730(false) {
    m_unknown670 = this;
    m_unknown731 = false;
}

CAIObject::~CAIObject() {
    m_unknown0c0 = false;
    if (m_target) {
        m_unknown6d8 = false;
        CAIObject* old = m_target;
        m_target = 0;
        if (m_parameters.m_unknown28 == 8 || m_parameters.m_unknown28 == 2) {
            if (m_unknown2d8 == 1 || m_unknown2d8 == 2)
                m_unknown2d8 = 3;
        }
        if (old)
            AdjustTargetedCount(old, -1);
    }
    RemoveFromGlobalList();
    CSearchNode* node = (CSearchNode*)m_searchNodes.m_head;
    while (node) {
        m_searchNodes.RemoveElement(node);
        delete node;
        node = (CSearchNode*)m_searchNodes.m_head;
    }
}
