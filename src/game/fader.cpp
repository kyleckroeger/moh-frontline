// The end of fader.cpp (0x8007bfa4): CFader's SetColor (the fade colour
// stored), Update (the current control updated, then dropped once it stops
// fading), Init (the render list kept and the fader's render bin registered
// with it), the weak MathFunClamp<int> instance, and the static
// initialisation of the fade colour and the render bin. The fader bin, the
// fade control's Update and CFader::SetControl before these are in other
// units or not reconstructed. CFader, CFaderControl, CFaderBin, CRenderBin,
// CRenderList, CColor and the template are named by the mangled symbols;
// members, the base layout, the 114 bin priority and the colour constructor
// are inferred. CFaderBin declares its Link override (defined elsewhere)
// first so its global table is not emitted here; CRenderBin's weak table and
// inline destructor are weak duplicates.
class CDmaTag;
class CDmaPacket;

struct CColor {
    CColor() {}
    CColor(unsigned char ar, unsigned char ag, unsigned char ab, unsigned char aa) : r(ar), g(ag), b(ab), a(aa) {}
    CColor(const CColor& c) : r(c.r), g(c.g), b(c.b), a(c.a) {}

    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

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

class CFaderBin : public CRenderBin {
public:
    virtual void Link(CDmaTag*, CDmaPacket&);
    CFaderBin() { m_priority = 114; }
    virtual ~CFaderBin();
};

class CFaderControl {
public:
    virtual void Update(float);
    bool IsFading() const;

    CColor m_from;
    CColor m_to;
    float m_duration;
    float m_time;
};

inline bool CFaderControl::IsFading() const {
    return m_time <= m_duration;
}

class CFader {
public:
    static void Init(CRenderList*);
    static void Update(float);
    static void SetColor(CColor);
};

static CFaderControl* g_pFaderControl;
static CRenderList* g_pRenderList;
static CColor g_color(0, 0, 0, 0);
static CFaderBin g_FaderBin;

void CFader::SetColor(CColor color) {
    g_color = color;
}

void CFader::Update(float elapsed) {
    if (g_pFaderControl) {
        g_pFaderControl->Update(elapsed);
        if (!g_pFaderControl->IsFading())
            g_pFaderControl = 0;
    }
}

void CFader::Init(CRenderList* list) {
    g_pRenderList = list;
    list->Register(g_FaderBin);
}

template <class T> __declspec(weak) T MathFunClamp(T value, T low, T high) {
    if (value < low)
        return low;
    if (value > high)
        return high;
    return value;
}

template int MathFunClamp<int>(int, int, int);
