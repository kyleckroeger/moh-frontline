// CScreen (0x8001b7e8): the empty PutDisp, Clear (copying the display to the
// buffer at +248), the clear rectangle and colour getters, GetClear (false
// while capturing), the depth buffer's maximum by depth format (16, 24 or 28
// bits), the depth and frame formats (returned by address), the width and
// height, the clear rectangle, colour and flag setters and the vertical sync
// count. CScreen, CRect, CColor and the functions are named by the mangled
// symbols; the members, the format records (their first word the format
// type), the rectangle and colour layouts and the result types are inferred.
// The rest of the file is not part of this unit.
struct CRect {
    int left;
    int top;
    int right;
    int bottom;
};

struct CColor {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

extern "C" void GXCopyDisp(void*, int);

extern bool g_bCapture;
extern unsigned long g_FrameNum;

struct ScreenFormatView {
    int type;
};

class CScreen {
public:
    void PutDisp();
    void Clear();
    CRect* GetClearRect();
    CColor* GetClearColor();
    bool GetClear();
    unsigned long GetDepthBufferMax();
    void SetClearRect(const CRect&);
    void SetClearColor(const CColor&);
    void SetClear(bool);
    unsigned long GetVSyncs();
    ScreenFormatView* GetDepthFormat();
    ScreenFormatView* GetFrameFormat();
    int GetWidth();
    int GetHeight();

    unsigned char unknown00[5];
    bool m_clear;
    unsigned char unknown06[2];
    CRect m_clearRect;
    CColor m_clearColor;
    unsigned char unknown1c[4];
    int m_width;
    int m_height;
    unsigned char unknown28[8];
    ScreenFormatView m_frameFormat;
    ScreenFormatView m_depthFormat;
    unsigned char unknown38[192];
    void* m_copyBuffer;
};

void CScreen::PutDisp() {
}

void CScreen::Clear() {
    GXCopyDisp(m_copyBuffer, 1);
}

CRect* CScreen::GetClearRect() {
    return &m_clearRect;
}

CColor* CScreen::GetClearColor() {
    return &m_clearColor;
}

bool CScreen::GetClear() {
    if (g_bCapture)
        return false;
    return m_clear;
}

unsigned long CScreen::GetDepthBufferMax() {
    switch (m_depthFormat.type) {
    case 0:
        return 0xffff;
    case 1:
        return 0xffffff;
    case 2:
        return 0xfffffff;
    default:
        return 0xfffffff;
    }
}

ScreenFormatView* CScreen::GetDepthFormat() {
    return &m_depthFormat;
}

ScreenFormatView* CScreen::GetFrameFormat() {
    return &m_frameFormat;
}

int CScreen::GetWidth() {
    return m_width;
}

int CScreen::GetHeight() {
    return m_height;
}

void CScreen::SetClearRect(const CRect& rect) {
    m_clearRect = rect;
}

void CScreen::SetClearColor(const CColor& color) {
    m_clearColor = color;
}

void CScreen::SetClear(bool clear) {
    m_clear = clear;
}

unsigned long CScreen::GetVSyncs() {
    return g_FrameNum;
}
