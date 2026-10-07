// A fragment of fader.cpp (0x8007c024): CFader::Init, which keeps the render
// list and registers the fader's render bin with it, and the weak
// MathFunClamp<int> instance emitted after it (the value clamped to the
// bounds). The file name is this project's; the original record is fader.cpp
// and the functions around these are not reconstructed. CFader, CRenderList,
// CRenderBin and the template are named by the mangled symbols; the bin is an
// inferred view and the file's globals are extern. The file is compiled with
// deferred inlining, so the source lists functions in reverse image order;
// the template is defined __declspec(weak) and instantiated explicitly.
struct CColor {
    CColor() {}
    CColor(const CColor& c) : r(c.r), g(c.g), b(c.b), a(c.a) {}

    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

class CRenderBin;

class CRenderList {
public:
    void Register(CRenderBin&);
};

class CFaderControl {
public:
    CFaderControl(CColor, CColor, float);
    virtual void Update(float);
    bool IsFading() const;

    CColor m_from;
    CColor m_to;
    float m_duration;
    float m_time;
};

class CFader {
public:
    static void Init(CRenderList*);
    static void Update(float);
    static void SetColor(CColor);
    static void SetControl(CFaderControl*);
};

// File-local in the original; declared without static here because this
// fragment does not define them (the manifest lists them as local
// externals). g_color and g_FaderBin are constructed by the file's static
// initialisation, which is not part of this unit.
extern char g_FaderBin[];
extern CColor g_color;

extern CFaderControl* g_pFaderControl;
extern CRenderList* g_pRenderList;

void CFader::Init(CRenderList* list) {
    g_pRenderList = list;
    list->Register(*(CRenderBin*)g_FaderBin);
}

void CFader::Update(float elapsed);

void CFader::SetColor(CColor color);

void CFader::SetControl(CFaderControl* control);

CFaderControl::CFaderControl(CColor from, CColor to, float duration);

bool CFaderControl::IsFading() const;

template <class T> __declspec(weak) T MathFunClamp(T value, T low, T high) {
    if (value < low)
        return low;
    if (value > high)
        return high;
    return value;
}

template int MathFunClamp<int>(int, int, int);
