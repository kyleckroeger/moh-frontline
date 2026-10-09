// The end of fader.cpp (0x8007bf38): CFader's SetControl (the control's time
// reset and its start colour made the fade colour), SetColor (the fade colour
// stored), Update (the current control updated, then dropped once it stops
// fading), Init (the render list kept and the fader's render bin registered
// with it), the weak MathFunClamp<int> instance, and the static
// initialisation of the fade colour and the render bin. SetControl inlines
// SetColor, which the image places after it, so the file is compiled with
// deferred inlining and the functions are written in reverse image order.
// The fader bin and the fade control before these are in other units.
// CFader, CFaderControl, CFaderBin, CRenderBin, CRenderList, CColor and the
// template are named by the mangled symbols; members, the base layout, the
// 114 bin priority, the colour constructor and assignment (bytewise) and
// SetControl building the colour from its components are inferred. CFaderBin
// declares its Link override (defined elsewhere) first so its global table
// is not emitted here; CRenderBin's weak table and inline destructor are weak
// duplicates.
class CDmaTag;
class CDmaPacket;

struct CColor {
    CColor() {}
    CColor(unsigned char ar, unsigned char ag, unsigned char ab, unsigned char aa) : r(ar), g(ag), b(ab), a(aa) {}
    CColor& operator=(const CColor& c) { r = c.r; g = c.g; b = c.b; a = c.a; return *this; }

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
    static void SetControl(CFaderControl*);
    static void SetColor(CColor);
};

static CFaderControl* g_pFaderControl;
static CRenderList* g_pRenderList;
static CColor g_color(0, 0, 0, 0);
static CFaderBin g_FaderBin;

template <class T> __declspec(weak) T MathFunClamp(T value, T low, T high) {
    if (value < low)
        return low;
    if (value > high)
        return high;
    return value;
}

template int MathFunClamp<int>(int, int, int);

void CFader::Init(CRenderList* list) {
    g_pRenderList = list;
    list->Register(g_FaderBin);
}

void CFader::Update(float elapsed) {
    if (g_pFaderControl) {
        g_pFaderControl->Update(elapsed);
        if (!g_pFaderControl->IsFading())
            g_pFaderControl = 0;
    }
}

void CFader::SetColor(CColor color) {
    g_color = color;
}

void CFader::SetControl(CFaderControl* control) {
    if (control) {
        control->m_time = 0.0f;
        SetColor(CColor(control->m_from.r, control->m_from.g, control->m_from.b, control->m_from.a));
    }
    g_pFaderControl = control;
}
