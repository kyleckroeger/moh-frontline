// CFontDriver, the functions at the start of the file: the static font-driver
// hooks that destroy (nothing to do) and create a font's texture for the
// current CFont. The class and structure names come from the mangled symbols;
// the members, the FONT view and CTexture's zeroing constructor are inferred
// from offsets. CFont is a non-virtual view here. EndDraw follows and is not
// matched (its colour copy goes through a word temporary on the stack; draft
// with StartDraw and Draw, which match, in scratch/lib/font_wip.cpp).
extern "C" void* memset(void*, int, unsigned long);

struct Shape;
class CTexture {
public:
    CTexture() { memset(this, 0, sizeof(CTexture)); }
    void Set(Shape*, bool);

    unsigned char unknown00[64];
    void* m_font;
    struct FONT* m_owner;
};

class CFont {
public:
    unsigned char unknown00[36];
    CTexture* m_texture;
};

struct FONT {
    unsigned char unknown00[28];
    int shapeOffset;
    unsigned char unknown20[80];
    CFont* font;
};

extern CFont* g_pCurrentFont;

class CFontDriver {
public:
    static void DestroyFont(FONT*);
    static void CreateFont(FONT*);
};

void CFontDriver::DestroyFont(FONT*) {
}

void CFontDriver::CreateFont(FONT* font) {
    if (g_pCurrentFont) {
        Shape* shape = (Shape*)((char*)font + font->shapeOffset);
        CTexture* texture = new CTexture;
        texture->Set(shape, false);
        texture->m_owner = font;
        g_pCurrentFont->m_texture = texture;
        texture->m_font = g_pCurrentFont;
        font->font = g_pCurrentFont;
    }
}
