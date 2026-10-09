// A fragment of decal.cpp (0x80083e08): the CBulletDecalTextureBin destructor
// (its virtual table pointer, then the inline CUcodeRenderBin and CRenderBin
// destructors, and the object freed when asked) and default constructor
// (CRenderBinData's inline constructor clears the bin's link words and sets
// +20 to 3 before the CRenderBin and CUcodeRenderBin table pointers; then the
// word at +36 is cleared). The file name is this project's; the original
// record is decal.cpp. CRenderBinData is named by the RTTI record of
// CRenderBin (its non-polymorphic base, which is why the 28 bytes precede the
// table pointer); the other classes and functions are named by the mangled
// symbols. CRenderBinData's members are inferred: CRenderBin::Init walks a
// child list from +8 through the siblings at +4, and IsUsed tests +16. Only
// the virtuals these functions need are declared. CBulletDecalTextureBin
// declares its Render override (defined elsewhere) first so its global
// virtual table is not emitted here. CRenderBin and CUcodeRenderBin have no
// key function: their tables are weak in the original, so the compiler's
// copies of the tables and of the inline base destructors are weak
// duplicates, linked to the original copies.
class CDmaTag;
class CDmaPacket;

class CRenderBin;

/* CRenderBin's non-polymorphic base, named by its RTTI record. */
struct CRenderBinData {
    CRenderBinData() : data00(0), m_next(0), m_children(0), data0c(0), data10(0), data14(3) {}

    int data00;
    CRenderBin* m_next;
    CRenderBin* m_children;
    int data0c;
    int data10;
    int data14;
    int data18;
};

class CRenderBin : public CRenderBinData {
public:
    CRenderBin() {}
    virtual ~CRenderBin() {}
};

class CUcodeRenderBin : public CRenderBin {
public:
    CUcodeRenderBin() {}
    virtual ~CUcodeRenderBin() {}
};

class CBulletDecalTextureBin : public CUcodeRenderBin {
public:
    virtual int Render(CDmaPacket&, void*);
    virtual ~CBulletDecalTextureBin();
    CBulletDecalTextureBin();

    int data20;
    int data24;
};

CBulletDecalTextureBin::~CBulletDecalTextureBin() {
}

CBulletDecalTextureBin::CBulletDecalTextureBin() {
    data24 = 0;
}
