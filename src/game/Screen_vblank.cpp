// A fragment of Screen.cpp (0x8001bf1c): the static CScreen::VBlankHandler
// counts the frame and, when there is a current screen and a buffer swap is
// pending, counts the retrace (more than one since the last swap marks the
// frame as late), shows the finished buffer (PutDisp) and draws into the other
// (SetDraw) through the table, flips the buffer index and clears the pending
// swap. The file name is this project's; the original record is Screen.cpp,
// after Screen_cscreen_dtor.cpp. The class, enums and function are named by the
// mangled symbols; CScreen's virtuals are declared in table order (as in
// Screen_fieldmode.cpp, with the destructor declared out of line so the table
// stays elsewhere); the members' names are inferred. The current screen, the
// frame counter and the swap flag are file-local in the original; this
// fragment declares them non-static so it links to the original objects.
class CColor;
class CRect;
enum EScreenWidth {};
enum EFrameFormat {};
enum EDepthFormat {};

class CScreen {
public:
    virtual ~CScreen();
    virtual void SetCurrent();
    virtual void Flip();
    virtual void Wait();
    virtual void SetClear(bool);
    virtual void SetClearColor(const CColor&);
    virtual void SetClearRect(const CRect&);
    virtual void GetHeight();
    virtual void GetWidth();
    virtual void GetHeightScale();
    virtual void GetWidthScale();
    virtual void GetFrameFormat();
    virtual void GetDepthFormat();
    virtual void GetClear();
    virtual void GetClearColor();
    virtual void GetClearRect();
    virtual void GetTMViewToScreen();
    virtual void Rebuild() = 0;
    virtual void PutDisp();
    virtual void Clear();
    virtual void SetDraw() = 0;
    virtual void _Capture();
    static void Capture();
    static void VBlankHandler(unsigned long);


    unsigned char data04;
    bool data05;
    int data08;
    int data0c;
    int data10;
    int data14;
    unsigned char data18;
    unsigned char data19;
    unsigned char data1a;
    unsigned char data1b;
    int data1c;
    EScreenWidth m_width;
    int m_height;
    unsigned char unknown28[8];
    EFrameFormat m_frameFormat;
    EDepthFormat m_depthFormat;
    unsigned char unknown38[28];
    int data54;
    int data58;
    unsigned char unknown5c[4];
    unsigned char unknown60[64];
    bool m_tmValid;
    int m_buffer;
    int m_retraces;
    bool m_late;
};

extern CScreen* g_pScreen;
extern unsigned long g_FrameNum;
extern bool g_bSwap;

void CScreen::VBlankHandler(unsigned long) {
    g_FrameNum++;
    if (g_pScreen && g_bSwap) {
        CScreen* screen = g_pScreen;
        if (screen->m_retraces++ > 1)
            screen->m_late = true;
        screen->PutDisp();
        screen->SetDraw();
        screen->m_buffer ^= 1;
        g_bSwap = false;
    }
}
