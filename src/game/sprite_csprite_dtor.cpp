// A fragment of sprite.cpp (0x800825b8): the CSprite destructor (its virtual table
// pointer; unregisters it from the render list and closes its file at +32 when there is one; then the inline CRenderBin destructor, and the object freed
// when asked). The file name is this project's; the original record is
// sprite.cpp. The classes, functions and the render list are named by the
// mangled symbols; the render-list pointer is file-local in the original (this
// fragment declares it non-static so it links to the original object), the
// member at +32 is inferred, and only the virtuals the destructor needs are
// declared. CSprite declares its Render override (defined elsewhere; the result
// type is not known) first so its global virtual table is not emitted here.
// CRenderBin's destructor is inline and its table weak in the original, so the
// compiler's copies are weak duplicates, linked to the original copies.
class CDmaPacket;
void TLT_CloseFile(void*);

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

class CSprite : public CRenderBin {
public:
    virtual void Render(CDmaPacket&, void*); /* result type not known */
    virtual ~CSprite();

    void* m_file;
};

CSprite::~CSprite() {
    g_pRenderList->UnRegister(*this);
    if (m_file) {
        TLT_CloseFile(m_file);
        m_file = 0;
    }
}
