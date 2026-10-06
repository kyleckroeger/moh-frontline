// CDualConfig's destructor: it deletes the three objects it holds, each
// checked for null before the delete. CDualConfig is named by the mangled
// symbols; the members are inferred from offsets. The pointers are deleted
// through a virtual destructor at the slot CRenderBin's sprites and fonts
// use, so they are viewed as CRenderBin pointers (their exact classes are not
// known). CRenderBin is an inferred view: 28 bytes of members, then its
// virtual table pointer, with the virtual destructor in the first slot.
class CRenderBin {
public:
    unsigned char unknown00[28];

    virtual ~CRenderBin();
};

class CDualConfig {
public:
    ~CDualConfig();

    CRenderBin* m_bin0;
    CRenderBin* m_bin4;
    CRenderBin* m_bin8;
};

CDualConfig::~CDualConfig() {
    if (m_bin0)
        delete m_bin0;
    if (m_bin8)
        delete m_bin8;
    if (m_bin4)
        delete m_bin4;
}
