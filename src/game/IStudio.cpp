// IStudio wrappers over the UI studio: activating a screen and loading one
// (with no extra parameters). IStudio and UISInfo_t are named by the symbols;
// the studio record pointer at the start of the object is inferred and the
// UIS functions' parameter types beyond the ids are left generic. The rest of
// the file is not part of this unit.
struct UISInfo_t;

extern "C" {
int UISSetScreenActive(UISInfo_t*, unsigned short, unsigned short);
int UISLoadScreen(UISInfo_t*, unsigned short, unsigned short, int, int);
}

class IStudio {
public:
    void iSirenActivateScreen(unsigned short, unsigned short);
    void iSirenLoadScreen(unsigned short, unsigned short);

    UISInfo_t* m_info;
};

void IStudio::iSirenActivateScreen(unsigned short group, unsigned short id) {
    UISSetScreenActive(m_info, group, id);
}

void IStudio::iSirenLoadScreen(unsigned short group, unsigned short id) {
    UISLoadScreen(m_info, group, id, 0, 0);
}
