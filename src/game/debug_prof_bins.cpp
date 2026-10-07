// A fragment of debug_prof.cpp (0x800f451c): the profiling history and debug
// statistics render bins' Render functions, which both call two of the
// screen's virtual functions and return 0. CProfHistRenderBin,
// CDbgStatsRenderBin, CDmaPacket and the screen pointer are named by the
// symbols; the bins are non-virtual views and the screen's virtual slots
// (+36, +40) are inferred views with unknown names. The bins' destructor and
// the static initialisation after this are not part of this unit.
class CDmaPacket;

class CScreenView {
public:
    virtual void unknown08();
    virtual void unknown0c();
    virtual void unknown10();
    virtual void unknown14();
    virtual void unknown18();
    virtual void unknown1c();
    virtual void unknown20();
    virtual void unknown24();
    virtual void unknown28();
};

extern CScreenView* g_pScreen;

class CProfHistRenderBin {
public:
    int Render(CDmaPacket&, void*);
};

class CDbgStatsRenderBin {
public:
    int Render(CDmaPacket&, void*);
};

int CProfHistRenderBin::Render(CDmaPacket&, void*) {
    g_pScreen->unknown28();
    g_pScreen->unknown24();
    return 0;
}

int CDbgStatsRenderBin::Render(CDmaPacket&, void*) {
    g_pScreen->unknown28();
    g_pScreen->unknown24();
    return 0;
}
