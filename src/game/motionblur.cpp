// The motion-blur bin's queries and set-up: IsUsed reports whether a blur
// amount is set, Link does nothing, SetAmount stores the amount and Init
// registers the blur bin with the render list. CRenderBin, CBlurBin,
// CMotionBlur and CRenderList are named by the mangled symbols; the bin and
// the file's statics are opaque or inferred views here, and the classes are
// non-virtual views (the bin's destructor before these functions and the
// static initialiser after them are not part of this unit; a draft of the
// whole file is in scratch/os/motionblur_best.cpp).
class CDmaTag;
class CDmaPacket;

class CRenderBin {
public:
    unsigned char data[32];
};

class CRenderList {
public:
    void Register(CRenderBin&);
};

class CBlurBin : public CRenderBin {
public:
    bool IsUsed();
    void Link(CDmaTag*, CDmaPacket&);
};

class CMotionBlur {
public:
    static void SetAmount(float);
    static void Init(CRenderList*);
};

extern CRenderList* g_pRenderList;
extern float g_BlurAmount;
extern CBlurBin g_BlurBin;

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
