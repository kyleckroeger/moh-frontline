// A fragment of staticmesh.cpp (0x800f1dfc): CMshSimpleBin's Init (its used
// flag cleared, then CRenderBin's inline Init: tag words cleared and the
// children re-initialised), IsUsed (the flag) and Link (nothing). The file
// name is this project's; the original record is staticmesh.cpp. The classes
// and functions are named by the mangled symbols, CRenderBinData by
// CRenderBin's RTTI record; the members are inferred (as in
// compartment_shadowbins.cpp). CMshSimpleBin declares its destructor (weak,
// emitted elsewhere in the file) first so its table is not emitted here; the
// compiler's copy of CRenderBin's inline Init is a weak duplicate.
class CDmaTag;
class CDmaPacket;
class CRenderBin;

/* CRenderBin's non-polymorphic base (see dmesh_cremapbin_ctor.cpp). */
struct CRenderBinData {
    int data00;
    CRenderBin* m_next;
    CRenderBin* m_children;
    int data0c;
    int data10;
    int m_priority;
    int data18;
};

/* CRenderBin's virtuals in table order (Init is slot 16). */
class CRenderBin : public CRenderBinData {
public:
    virtual ~CRenderBin() {}
    virtual int Render(CDmaPacket&, void*);
    virtual void Init() {
        data0c = 0;
        data10 = 0;
        for (CRenderBin* child = m_children; child; child = child->m_next)
            child->Init();
    }
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed() { return data10 != 0; }
};

class CMshSimpleBin : public CRenderBin {
public:
    virtual ~CMshSimpleBin();
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();

    bool m_used;
};

void CMshSimpleBin::Init() {
    m_used = false;
    CRenderBin::Init();
}

bool CMshSimpleBin::IsUsed() {
    return m_used;
}

void CMshSimpleBin::Link(CDmaTag*, CDmaPacket&) {
}
