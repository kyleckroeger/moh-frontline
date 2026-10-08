// A fragment of dmesh.cpp (0x800f6ab0): the CPartBin destructor (its virtual table
// pointer, which follows 28 bytes of members, then each base's through the
// inline destructors of CRenderBin, CPart, and the object freed when asked). The file
// name is this project's; the original record is dmesh.cpp. The classes and
// the destructor are named by the mangled symbols; the layout before the
// table pointer is not known, and only the virtuals the destructor needs are
// declared. CPartBin declares another of its virtual functions (defined
// elsewhere; the result type is not known) before its destructor, so its own
// global virtual table is not emitted here.
// CRenderBin: weak table (no key function) and inline destructor, so
// the compiler's copies are weak duplicates, linked to the original copies.
// CPart has a global table (keyed on another of its virtual functions,
// declared first here) and an inline destructor (weak in the original), which
// is only inlined here (its table is not emitted, so no copy is).
class CDmaPacket;

class CRenderBin {
    unsigned char unknown00[28]; /* members before the table pointer */

public:
    virtual ~CRenderBin() {}
};

class CPart : public CRenderBin {
public:
    virtual void Render(CDmaPacket&, void*); /* result type not known */
    virtual ~CPart() {}
};

class CPartBin : public CPart {
public:
    virtual void Render(CDmaPacket&, void*); /* result type not known */
    virtual ~CPartBin();
};

__declspec(weak) CPartBin::~CPartBin() {
}
