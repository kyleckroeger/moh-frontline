// The pause screen's destructor and constructor, the last functions of the
// file. Pause and IStudio are named by the mangled symbols; the members are
// inferred from offsets. The three pointers at the start are deleted through
// a virtual destructor at the slot CRenderBin's sprites and fonts use, so they
// are viewed as CRenderBin pointers (their exact classes are not known).
// CRenderBin is an inferred view: 28 bytes of members, then its virtual table
// pointer, with the virtual destructor in the first slot.
class CRenderBin {
public:
    unsigned char unknown00[28];

    virtual ~CRenderBin();
};

class IStudio {
public:
    IStudio();
    ~IStudio();

    unsigned char unknown00[20];
};

class Pause {
public:
    Pause();
    ~Pause();

    CRenderBin* m_bin0;
    CRenderBin* m_bin4;
    CRenderBin* m_bin8;
    unsigned char unknown0c[96];
    bool m_paused;
    unsigned char unknown6d[3];
    IStudio m_studio;
    unsigned char unknown84[16];
    bool m_flag94;
    bool m_flag95;
};

Pause::~Pause() {
    delete m_bin8;
    delete m_bin4;
    delete m_bin0;
}

Pause::Pause() : m_paused(false) {
    m_bin0 = 0;
    m_flag94 = true;
    m_flag95 = false;
}
