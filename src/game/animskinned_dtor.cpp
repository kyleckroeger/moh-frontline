// A fragment of animskinned.cpp (0x80066e3c): the CAnimSkinned destructor (its virtual table
// pointer then each base's through the
// inline destructors of CAnimated, and the object freed when asked). The file
// name is this project's; the original record is animskinned.cpp. The classes and
// the destructor are named by the mangled symbols; the layout before the
// table pointer is not known, and only the virtuals the destructor needs are
// declared. CAnimSkinned declares another of its virtual functions (defined
// elsewhere; the result type is not known) before its destructor, so its own
// global virtual table is not emitted here.
// CAnimated has a global table (keyed on another of its virtual functions,
// declared first here) and an inline destructor (weak in the original), which
// is only inlined here (its table is not emitted, so no copy is).
class CAnimated {
public:
    virtual void UpdateAnimation(float); /* result type not known */
    virtual ~CAnimated() {}
};

class CAnimSkinned : public CAnimated {
public:
    virtual void UpdateAnimation(float); /* result type not known */
    virtual ~CAnimSkinned();
};

CAnimSkinned::~CAnimSkinned() {
}
