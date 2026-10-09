// CDualConfig's destructor: it deletes the three objects it holds, each
// checked for null before the delete. CDualConfig is named by the mangled
// symbols; the members are inferred from offsets. The pointers are deleted
// through a virtual destructor at the slot CRenderBin's sprites and fonts
// use, so they are viewed as CRenderBin pointers (their exact classes are not
// known). CRenderBin is an inferred view: 28 bytes of members, then its
// virtual table pointer, with the virtual destructor in the first slot. The
// unit ends with the file's static initialisation: the global controller
// configuration is built (its members cleared, two flags set to 1, two
// floats to 0.0 from the .sdata2 pool; inferred) and its destructor
// registered.
class CRenderBin {
public:
    unsigned char unknown00[28];

    virtual ~CRenderBin();
};

class CDualConfig {
public:
    CDualConfig() {
        data10 = 1;
        data14 = 0;
        data18 = 0;
        data0c = false;
        m_bin4 = 0;
        data0d = false;
        data0e = false;
        m_bin0 = 0;
        data2c = 0.0f;
        data30 = 0.0f;
        data38 = 0;
        data40 = 1;
    }
    ~CDualConfig();

    CRenderBin* m_bin0;
    CRenderBin* m_bin4;
    CRenderBin* m_bin8;
    bool data0c;
    bool data0d;
    bool data0e;
    int data10;
    int data14;
    int data18;
    unsigned char unknown1c[16];
    float data2c;
    float data30;
    unsigned char unknown34[4];
    int data38;
    unsigned char unknown3c[4];
    int data40;
};

CDualConfig::~CDualConfig() {
    if (m_bin0)
        delete m_bin0;
    if (m_bin8)
        delete m_bin8;
    if (m_bin4)
        delete m_bin4;
}

CDualConfig g_ControllerConfig;
