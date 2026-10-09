// A fragment of Screen.cpp (0x8001b4b0): CScreenFieldMode's empty PutDisp,
// SetDraw and Rebuild, then the CScreenFieldMode destructor (its
// virtual table pointer, then CScreen's destructor inlined: its table pointer
// and the file-local current or previous screen released when it is this one;
// then the object freed when asked). The file
// name is this project's; the original record is Screen.cpp. The classes and
// the destructor are named by the mangled symbols; the layout before the
// table pointer is not known, and only the virtuals the destructor needs are
// declared. CScreenFieldMode declares another of its virtual functions (defined
// elsewhere; the result type is not known) before its destructor, so its own
// global virtual table is not emitted here.
// The destructor of CScreen is global in the original (out of line,
// elsewhere in the same file) yet inlined here; how the original made it available
// inline is not known, so the view declares it inline, after another of
// the class's own virtual functions so that its global table is not emitted.
class CScreen;
extern CScreen* g_pScreen;
extern CScreen* g_prevScreen;

class CScreen {
public:
    virtual void SetCurrent(); /* result type not known */
    virtual ~CScreen() {
        if (g_pScreen == this)
            g_pScreen = g_prevScreen;
        else if (g_prevScreen == this)
            g_prevScreen = 0;
    }
};

class CScreenFieldMode : public CScreen {
public:
    virtual void SetCurrent(); /* result type not known */
    virtual ~CScreenFieldMode();
    virtual void Rebuild();
    virtual void PutDisp();
    virtual void SetDraw();
};

void CScreenFieldMode::PutDisp() {
}

void CScreenFieldMode::SetDraw() {
}

void CScreenFieldMode::Rebuild() {
}

CScreenFieldMode::~CScreenFieldMode() {
}
