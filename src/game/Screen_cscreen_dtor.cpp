// A fragment of Screen.cpp (0x8001bea4): the CScreen destructor (its virtual
// table pointer; the current screen falls back to the previous one when it is
// this screen, or the previous screen is cleared when it is this one; then
// the object freed when asked). The file name is this
// project's; the original record is Screen.cpp. The class and the destructor
// are named by the mangled symbols, and only the virtuals the destructor
// needs are declared. CScreen declares another of its virtual functions
// (defined elsewhere; the result type is not known) before its destructor,
// so its global virtual table is not emitted here.
class CScreen {
public:
    virtual void SetCurrent(); /* result type not known */
    virtual ~CScreen();
};

extern CScreen* g_pScreen;
extern CScreen* g_prevScreen;

CScreen::~CScreen() {
    if (g_pScreen == this)
        g_pScreen = g_prevScreen;
    else if (g_prevScreen == this)
        g_prevScreen = 0;
}
