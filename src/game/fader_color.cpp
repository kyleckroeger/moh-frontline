// A fragment of fader.cpp (0x8007bfa4): CFader::SetColor, which stores the
// fade colour. The file name is this project's; the original record is
// fader.cpp and the functions around it are not reconstructed. CFader and
// CColor are named by the mangled symbols; the file's globals are extern.
// Screen fades: the current fade control (a colour pair and a duration with
// a virtual Update) and the fader's colour, applied by the fader render bin.
// CFader, CFaderControl, CFaderBin, CColor and CRenderList are named by the
// mangled symbols and RTTI; members are inferred from offsets. The file is
// compiled with deferred inlining (SetControl inlines SetColor, which follows
// it in the image), so the source lists functions in reverse image order.
// This unit covers IsFading to CFader::Init; the bin, CFaderControl::Update,
// the MathFunClamp instantiation and the static initialisation are not part
// of it.
// Screen fades: the current fade control (a colour pair and a duration with
// a virtual Update) and the fader's colour, applied by the fader render bin.
// CFader, CFaderControl, CFaderBin, CColor and CRenderList are named by the
// mangled symbols and RTTI; members are inferred from offsets. The file is
// compiled with deferred inlining (SetControl inlines SetColor, which follows
// it in the image), so the source lists functions in reverse image order.
// This unit covers IsFading to CFader::Init; the bin, CFaderControl::Update,
// the MathFunClamp instantiation and the static initialisation are not part
// of it.
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

void CFader::Init(CRenderList* list);

void CFader::Update(float elapsed);

void CFader::SetColor(CColor color) {
    g_color = color;
}

void CFader::SetControl(CFaderControl* control);

CFaderControl::CFaderControl(CColor from, CColor to, float duration);

bool CFaderControl::IsFading() const;


