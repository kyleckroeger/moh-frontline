// A fragment of Screen.cpp (0x8001be68): CScreen::GetCurrent (the current
// screen), Init (the file's ready flag set, the video interface initialised
// and the vertical-blank handler installed as the pre-retrace callback), and
// the CScreen destructor (its virtual
// table pointer; the current screen falls back to the previous one when it is
// this screen, or the previous screen is cleared when it is this one; then
// the object freed when asked). The file name is this
// project's; the original record is Screen.cpp. The class and the destructor
// are named by the mangled symbols, and only the virtuals the destructor
// needs are declared. CScreen declares another of its virtual functions
// (defined elsewhere; the result type is not known) before its destructor,
// so its global virtual table is not emitted here.
extern "C" {
void VIInit(void);
void* VISetPreRetraceCallback(void (*)(unsigned long));
}

class CScreen {
public:
    virtual void SetCurrent(); /* result type not known */
    virtual ~CScreen();
    static CScreen* GetCurrent();
    static void Init();
    static void VBlankHandler(unsigned long);
};

extern CScreen* g_pScreen;
extern CScreen* g_prevScreen;
extern bool g_bReady;

CScreen* CScreen::GetCurrent() {
    return g_pScreen;
}

void CScreen::Init() {
    g_bReady = true;
    VIInit();
    VISetPreRetraceCallback(VBlankHandler);
}

CScreen::~CScreen() {
    if (g_pScreen == this)
        g_pScreen = g_prevScreen;
    else if (g_prevScreen == this)
        g_prevScreen = 0;
}
