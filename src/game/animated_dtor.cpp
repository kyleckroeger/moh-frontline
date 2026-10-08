// A fragment of animated.cpp (0x800609a0): the weak CAnimated destructor (its virtual
// table pointer, then the object freed when asked). The file name is this
// project's; the original record is animated.cpp. The class and the destructor
// are named by the mangled symbols, and only the virtuals the destructor
// needs are declared. CAnimated declares another of its virtual functions
// (defined elsewhere; the result type is not known) before its destructor,
// so its global virtual table is not emitted here.
class CAnimated {
public:
    virtual void UpdateAnimation(float); /* result type not known */
    virtual ~CAnimated();
};

__declspec(weak) CAnimated::~CAnimated() {
}
