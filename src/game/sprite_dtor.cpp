// A fragment of sprite.cpp (0x800827bc): the CSpriteNoTextureBin destructor (its virtual table
// pointer, which follows 28 bytes of members, then each base's through the
// inline destructors of CRenderBin, and the object freed when asked). The file
// name is this project's; the original record is sprite.cpp. The classes and
// the destructor are named by the mangled symbols; the layout before the
// table pointer is not known, and only the virtuals the destructor needs are
// declared. CSpriteNoTextureBin declares an override defined elsewhere before its
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

class CSpriteNoTextureBin : public CRenderBin {
public:
    virtual int Render(CDmaPacket&, void*);
    virtual ~CSpriteNoTextureBin();
};

__declspec(weak) CSpriteNoTextureBin::~CSpriteNoTextureBin() {
}
