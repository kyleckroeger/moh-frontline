// A fragment of font.cpp (0x8007d0c0): the CFont destructor (its virtual table
// pointer; unregisters it from the render list, destroys its font handle at +32 and clears it; then the inline CRenderBin destructor, and the object freed
// when asked). The file name is this project's; the original record is
// font.cpp. The classes, functions and the render list are named by the
// mangled symbols; the render-list pointer is file-local in the original (this
// fragment declares it non-static so it links to the original object), the
// member at +32 is inferred, and only the virtuals the destructor needs are
// declared. CFont declares its Render override (defined elsewhere; the result
// type is not known) first so its global virtual table is not emitted here.
// CRenderBin's destructor is inline and its table weak in the original, so the
// compiler's copies are weak duplicates, linked to the original copies.
class CDmaPacket;
extern "C" void FONT_destroy(void*);

class CRenderBin {
    unsigned char unknown00[28]; /* members before the table pointer */

public:
    virtual ~CRenderBin() {}
};

class CRenderList {
public:
    void UnRegister(CRenderBin&);
};

extern CRenderList* g_pRenderList;

class CFont : public CRenderBin {
public:
    virtual void Render(CDmaPacket&, void*); /* result type not known */
    virtual ~CFont();

    void* m_font;
};

CFont::~CFont() {
    g_pRenderList->UnRegister(*this);
    FONT_destroy(m_font);
    m_font = 0;
}
