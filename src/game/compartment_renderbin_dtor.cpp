// A fragment of compartment.cpp (0x80074290): CRenderBin's weak destructor
// (the table pointer reset and the object freed when asked; the original copy
// that the other units' weak duplicates link to). The file name is this
// project's; the original record is compartment.cpp. CRenderBin and its
// destructor are named by the mangled symbols; the members before the table
// pointer are not part of this view. Defining the weak destructor out of line
// makes it CRenderBin's key function, so the compiler emits a (global) copy of
// CRenderBin's table; the original table is weak, so that copy is a weak
// duplicate, linked to the original.
class CDmaPacket;
class CDmaTag;

class CRenderBin {
    unsigned char unknown00[28]; /* members before the table pointer */

public:
    virtual ~CRenderBin();
    virtual int Render(CDmaPacket&, void*);
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();
};

__declspec(weak) CRenderBin::~CRenderBin() {
}
