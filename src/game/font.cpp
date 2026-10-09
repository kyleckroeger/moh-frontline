// The end of font.cpp (0x8007d1d4): the static initialisation of the file's
// string packet (its colour set to (0, 0, 0, 128)) and of the font driver,
// whose hooks are CFontDriver's Draw, StartDraw, EndDraw, CreateFont and
// DestroyFont (its destructor registered). The rest of the file is in other
// units. CFontDriver, its functions, FONT, FONTCHAR, stringPacket and
// g_FontDriver are named by the symbols; the object sizes come from the
// symbols, while the hook members, the packet view (its type name is not
// known: StringPacketView is this project's) and the colour constructor are
// inferred.
struct FONT;
struct FONTCHAR;

struct CColor {
    CColor(unsigned char ar, unsigned char ag, unsigned char ab, unsigned char aa) : r(ar), g(ag), b(ab), a(aa) {}

    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

struct StringPacketView {
    StringPacketView() : m_color(0, 0, 0, 128) {}

    unsigned char unknown000[12];
    CColor m_color;
    unsigned char unknown010[4096];
};

class CFontDriver {
public:
    CFontDriver()
        : m_draw(Draw), m_startDraw(StartDraw), m_endDraw(EndDraw), m_createFont(CreateFont),
          m_destroyFont(DestroyFont) {}
    ~CFontDriver();
    static void Draw(FONT*, const FONTCHAR*, float, float);
    static void StartDraw(FONT*);
    static void EndDraw(FONT*);
    static void CreateFont(FONT*);
    static void DestroyFont(FONT*);

    void (*m_draw)(FONT*, const FONTCHAR*, float, float);
    void (*m_startDraw)(FONT*);
    void (*m_endDraw)(FONT*);
    void (*m_createFont)(FONT*);
    void (*m_destroyFont)(FONT*);
};

static StringPacketView stringPacket;
static CFontDriver g_FontDriver;
