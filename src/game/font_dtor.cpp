// A fragment of font.cpp (0x8007d0c0): the CFont destructor and constructor
// (CRenderBinData's inline constructor and the table pointers; the priority
// stored; the font created from the name while the file-local current font
// points at this one; the bin registered with the render list). The
// destructor (its virtual table
// pointer; unregisters it from the render list, destroys its font handle at +32 and clears it; then the inline CRenderBin destructor, and the object freed
// when asked). The file name is this project's; the original record is
// font.cpp. The classes, functions and the render list are named by the
// mangled symbols (CRenderBinData by CRenderBin's RTTI record); the render-list
// and current-font pointers are file-local in the original (this
// fragment declares it non-static so it links to the original object), the
// member at +32 is inferred, and only the virtuals the destructor needs are
// declared. CFont declares its Render override (defined elsewhere; the result
// type is not known) first so its global virtual table is not emitted here.
// CRenderBin's destructor is inline and its table weak in the original, so the
// compiler's copies are weak duplicates, linked to the original copies.
class CDmaTag;
class CDmaPacket;
extern "C" void FONT_destroy(void*);
extern "C" void* FONT_create(const char*);
enum ERenderPriority {};

class CRenderBin;

/* CRenderBin's non-polymorphic base, named by CRenderBin's RTTI record; the
   members are inferred (CRenderBin::Init walks children from +8 through the
   siblings at +4, IsUsed tests +16, +20 is the render priority, 3 by
   default). */
struct CRenderBinData {
    CRenderBinData() : data00(0), m_next(0), m_children(0), data0c(0), data10(0), m_priority(3) {}

    int data00;
    CRenderBin* m_next;
    CRenderBin* m_children;
    int data0c;
    int data10;
    int m_priority;
    int data18;
};

/* CRenderBin's virtuals in table order; all are weak (inline) in the
   original. Init's and IsUsed's bodies are not part of this view, so they
   are declared without one, which also keeps CRenderBin's table out of this
   unit. */
class CRenderBin : public CRenderBinData {
public:
    CRenderBin() {}
    virtual ~CRenderBin() {}
    virtual int Render(CDmaPacket&, void*) { return 0; }
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&) {}
    virtual bool IsUsed();
};


class CRenderList {
public:
    void Register(CRenderBin&);
    void UnRegister(CRenderBin&);
};

class CFont;
extern CRenderList* g_pRenderList;
extern CFont* g_pCurrentFont;

class CFont : public CRenderBin {
public:
    virtual void Render(CDmaPacket&, void*); /* result type not known */
    virtual ~CFont();
    CFont(const char*, ERenderPriority);

    void* m_font;
};

CFont::~CFont() {
    g_pRenderList->UnRegister(*this);
    FONT_destroy(m_font);
    m_font = 0;
}

CFont::CFont(const char* name, ERenderPriority priority) {
    m_priority = priority;
    g_pCurrentFont = this;
    m_font = FONT_create(name);
    g_pCurrentFont = 0;
    g_pRenderList->Register(*this);
}
