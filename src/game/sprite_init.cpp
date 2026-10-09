// A fragment of sprite.cpp (0x80081894); the file name is this project's. CSprite, the first functions of the file: Init clears two counters and
// initialises every render bin in the list it holds, and Unload closes the
// sprite's loaded file. CSprite, CRenderBin, CDmaTag and CDmaPacket are named
// by the mangled symbols; CSprite is a non-virtual view with members inferred
// from offsets. CRenderBin's virtual functions are declared in the order of
// __vt__10CRenderBin (destructor, Render, Init, Link, IsUsed), with its links
// before and its virtual table pointer after 28 bytes of members. Init is a
// weak symbol in the target (inline in the class), so it is defined
// __declspec(weak).
class CDmaTag;
class CDmaPacket;

void TLT_CloseFile(void*);

class CRenderBin {
public:
    CRenderBin* m_prev;
    CRenderBin* m_next;
    unsigned char unknown08[20];

    virtual ~CRenderBin();
    virtual void Render(CDmaPacket&, void*);
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();
};

class CSprite {
public:
    void Init();
    void Unload();

    unsigned char unknown00[8];
    CRenderBin* m_bins;
    int m_count0c;
    int m_count10;
    unsigned char unknown14[12];
    void* m_file;
};

__declspec(weak) void CSprite::Init() {
    m_count0c = 0;
    m_count10 = 0;
    for (CRenderBin* bin = m_bins; bin; bin = bin->m_next)
        bin->Init();
}

void CSprite::Unload() {
    if (m_file) {
        TLT_CloseFile(m_file);
        m_file = 0;
    }
}
