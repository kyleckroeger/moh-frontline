// The end of dmesh.cpp (0x800f69e4): CDMesh::InitClass (the render list
// kept, both mesh bins registered with it, each given its finish bin as its
// child, and the remap pool's next index cleared), the weak CPart::GetName
// ("DMESH Part"), the weak CRemapBin and CPartBin destructors (each base's
// table pointer through the inline destructors), and the static
// initialisation of the file's render bins and remap pool, with the array
// destructor the compiler generates for the pool. The two mesh bins
// (CUcodeRenderBin-based; priority 3 and 8, flag cleared) and the two finish
// bins are built through the inline CRenderBinData and CRenderBin
// constructors with their destructors registered, the 32 remap bins are
// built through CRemapBin's constructor (__construct_array) with the array
// destructor registered, and the perspective matrix's inline constructor
// initialises the matrix class once. The classes, functions and objects are
// named by the symbols; the objects' sizes come from the symbols, and the
// members, the bin constructors' priority parameter, the base layout and
// GetName's result type are inferred. Each bin declares an override defined
// elsewhere first so its global table stays elsewhere; CRemapBin's
// constructor is emitted elsewhere in the file, and the inline base
// destructors and tables are weak duplicates. The unit defines the file's
// .bss block; the render list and remap index are file-local .sbss objects
// declared extern here. The string links to its pool entry. Formerly the
// dmesh_part_name, dmesh_cremapbin_dtor and dmesh_cpartbin_dtor units. The
// rest of the file is in other units or not reconstructed.
class CDmaTag;
class CDmaPacket;

class CMatrix {
public:
    /* The default argument is not known; it does not change this code. */
    CMatrix(int unknown = 0) {
        if (!s_ClassInit)
            InitClass();
    }
    static void InitClass();
    static bool s_ClassInit;

    float m[4][4];
} __attribute__((aligned(16)));

class CRenderBinData {
public:
    CRenderBinData() : m_field0(0), m_field4(0), m_field8(0), m_fieldC(0), m_field10(0), m_priority(3) {}

    int m_field0;
    int m_field4;
    int m_field8;
    int m_fieldC;
    int m_field10;
    int m_priority;
    int m_field18;
};

class CRenderBin : public CRenderBinData {
public:
    virtual ~CRenderBin() {}
    virtual int Render(CDmaPacket&, void*);
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();
};

class CUcodeRenderBin : public CRenderBin {
public:
    virtual int Render(CDmaPacket&, void*);
    virtual ~CUcodeRenderBin() {}
};

class CDMRenderBin : public CUcodeRenderBin {
public:
    virtual void Link(CDmaTag*, CDmaPacket&);
    CDMRenderBin(int priority) {
        m_priority = priority;
        m_flag = false;
    }
    virtual ~CDMRenderBin();

    bool m_flag;
};

class CFinishPartBin : public CRenderBin {
public:
    virtual int Render(CDmaPacket&, void*);
    CFinishPartBin() {}
    virtual ~CFinishPartBin();
};

class CRemapBin : public CRenderBin {
public:
    virtual int Render(CDmaPacket&, void*);
    virtual ~CRemapBin();
    CRemapBin();

    unsigned char unknown20[8];
};

class CPart : public CRenderBin {
public:
    virtual void Render(CDmaPacket&, void*); /* result type not known */
    virtual ~CPart() {}
    const char* GetName();
};

class CPartBin : public CPart {
public:
    virtual void Render(CDmaPacket&, void*); /* result type not known */
    virtual ~CPartBin();
};

class CRenderList {
public:
    void Register(CRenderBin&);
};

class CDMesh {
public:
    static void InitClass(CRenderList*);
};

extern CRenderList* _renderList;
extern int _remap_next;

static CDMRenderBin _DMRenderBin(3);
static CDMRenderBin _DMRenderBinWeapon(8);
static CFinishPartBin _finish_bin;
static CFinishPartBin _finish_bin_weapon;
static CRemapBin _remap_pool[32];
static CMatrix g_PerspectiveMatrix;

void CDMesh::InitClass(CRenderList* list) {
    _renderList = list;
    _renderList->Register(_DMRenderBin);
    _renderList->Register(_DMRenderBinWeapon);
    _DMRenderBin.m_field8 = (int)&_finish_bin;
    _DMRenderBinWeapon.m_field8 = (int)&_finish_bin_weapon;
    _remap_next = 0;
}

__declspec(weak) const char* CPart::GetName() {
    return "DMESH Part";
}

__declspec(weak) CRemapBin::~CRemapBin() {
}

__declspec(weak) CPartBin::~CPartBin() {
}
