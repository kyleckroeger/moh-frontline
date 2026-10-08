// A fragment of fader.cpp (0x8007b914): the CFaderBin destructor (its virtual table
// pointer, which follows 28 bytes of members, then each base's through the
// inline destructors of CRenderBin, and the object freed when asked). The file
// name is this project's; the original record is fader.cpp. The classes and
// the destructor are named by the mangled symbols; the layout before the
// table pointer is not known, and only the virtuals the destructor needs are
// declared. CFaderBin declares an override defined elsewhere before its
// destructor, so its own (global) virtual table is not emitted here; the
// bases' tables are weak in the original (no key function), so the compiler's
// copies of them and of the inline base destructors are weak duplicates,
// linked to the original copies.
class CDmaTag;
class CDmaPacket;

class CRenderBin {
    unsigned char unknown00[28]; /* members before the table pointer */

public:
    virtual ~CRenderBin() {}
};

class CFaderBin : public CRenderBin {
public:
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual ~CFaderBin();
};

__declspec(weak) CFaderBin::~CFaderBin() {
}
