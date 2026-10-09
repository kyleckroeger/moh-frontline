// The end of sprite.cpp (0x80082818): the static initialisation of the
// file's untextured sprite bin (built through the inline CRenderBinData and
// CRenderBin constructors with priority 10, its destructor registered) and of
// g_DrawContext, whose colour is set to (0, 0, 0, 128). The rest of the file
// is in other units or not reconstructed. CSpriteNoTextureBin, CRenderBin,
// g_SpriteNoTexture and g_DrawContext are named by the symbols; members, the
// base layout, the 10 priority, the colour constructor and g_DrawContext's
// type are inferred (its type name is not known: DrawContextView is this
// project's). CSpriteNoTextureBin's virtuals are defined elsewhere, so its
// table is external; CRenderBin's inline destructor is a weak duplicate.
class CDmaTag;
class CDmaPacket;

struct CColor {
    CColor(unsigned char ar, unsigned char ag, unsigned char ab, unsigned char aa) : r(ar), g(ag), b(ab), a(aa) {}

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
    virtual int Render(CDmaPacket&, void*);
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();
};

class CSpriteNoTextureBin : public CRenderBin {
public:
    virtual int Render(CDmaPacket&, void*);
    CSpriteNoTextureBin() { m_priority = 10; }
    virtual ~CSpriteNoTextureBin();
    virtual void Link(CDmaTag*, CDmaPacket&);
};

struct DrawContextView {
    DrawContextView() : m_color(0, 0, 0, 128) {}

    unsigned char unknown00[28];
    CColor m_color;
};

static CSpriteNoTextureBin g_SpriteNoTexture;
static DrawContextView g_DrawContext;
