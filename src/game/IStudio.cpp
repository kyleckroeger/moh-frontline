// IStudio wrappers over the UI studio: the per-frame update (event processing,
// then the studio's idle pass) and draw pass (identity matrix, then the
// studio's objects), activating a screen and loading one (with no extra
// parameters). IStudio and UISInfo_t are named by the symbols;
// the studio record pointer at the start of the object is inferred and the
// UIS functions' parameter types beyond the ids are left generic. The rest of
// the file is not part of this unit.
struct UISInfo_t;

extern "C" {
int UISSetScreenActive(UISInfo_t*, unsigned short, unsigned short);
int UISLoadScreen(UISInfo_t*, unsigned short, unsigned short, int, int);
void UISIdleProcess(UISInfo_t*, unsigned long);
void UISDrawObjects(UISInfo_t*, unsigned long);
}

class CMatrixStack {
public:
    void Ident();
};

extern CMatrixStack* g_matStack;

class IStudio {
public:
    void iSirenUpdate(unsigned long);
    void iSirenDrawProcessFnc(unsigned long);
    void iSirenEventProcessFnc();
    void iSirenActivateScreen(unsigned short, unsigned short);
    void iSirenLoadScreen(unsigned short, unsigned short);

    UISInfo_t* m_info;
};

void IStudio::iSirenUpdate(unsigned long time) {
    iSirenEventProcessFnc();
    UISIdleProcess(m_info, time);
}

void IStudio::iSirenDrawProcessFnc(unsigned long time) {
    g_matStack->Ident();
    UISDrawObjects(m_info, time);
}

void IStudio::iSirenActivateScreen(unsigned short group, unsigned short id) {
    UISSetScreenActive(m_info, group, id);
}

void IStudio::iSirenLoadScreen(unsigned short group, unsigned short id) {
    UISLoadScreen(m_info, group, id, 0, 0);
}
