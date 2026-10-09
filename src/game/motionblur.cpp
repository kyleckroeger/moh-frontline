// The whole of motionblur.cpp: the motion-blur render bin, a CRenderBin
// registered with the render list whose IsUsed reports whether a blur amount
// is set (its destructor, IsUsed and an empty Link), CMotionBlur's SetAmount
// and Init, and the static initialisation of the bin. CRenderBinData,
// CRenderBin, CBlurBin and CMotionBlur are named by the mangled symbols and
// the RTTI; members, the 113 bin priority and the base layout are inferred.
// CBlurBin's destructor is its key function, so its table and type
// information are emitted here; CRenderBin's weak table and inline destructor
// are weak duplicates, linked to the original copies.
class CDmaTag;
class CDmaPacket;

class CRenderBinData {
public:
    CRenderBinData() : m_field0(0), m_field4(0), m_field8(0), m_fieldC(0), m_field10(0), m_priority(3) {}

    int m_field0;
    int m_field4;
    int m_field8;
    int m_fieldC;
    int m_field10;
    int m_priority;
    int m_field18;
};

class CRenderBin : public CRenderBinData {
public:
    virtual ~CRenderBin() {}
    virtual void Render(CDmaPacket&, void*);
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();
};

class CRenderList {
public:
    void Register(CRenderBin&);
};

class CBlurBin : public CRenderBin {
public:
    CBlurBin() { m_priority = 113; }
    virtual ~CBlurBin();
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();
};

class CMotionBlur {
public:
    static void SetAmount(float);
    static void Init(CRenderList*);
};

static CRenderList* g_pRenderList;
static float g_BlurAmount;
static CBlurBin g_BlurBin;

__declspec(weak) CBlurBin::~CBlurBin() {
}

bool CBlurBin::IsUsed() {
    return g_BlurAmount != 0.0f;
}

void CBlurBin::Link(CDmaTag*, CDmaPacket&) {
}

void CMotionBlur::SetAmount(float amount) {
    g_BlurAmount = amount;
}

void CMotionBlur::Init(CRenderList* list) {
    g_pRenderList = list;
    list->Register(g_BlurBin);
}
