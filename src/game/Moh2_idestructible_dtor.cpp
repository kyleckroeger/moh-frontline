// A fragment of Moh2.cpp (0x8001ae00): the weak IDestructible destructor (its virtual
// table pointer, then the object freed when asked). The file name is this
// project's; the original record is Moh2.cpp. The class and the destructor
// are named by the mangled symbols, and only the virtuals the destructor
// needs are declared. IDestructible declares another of its virtual functions
// (defined elsewhere; the result type is not known) before its destructor,
// so its global virtual table is not emitted here.
class IDestructible {
public:
    virtual void MarkForDestruction(int); /* result type not known */
    virtual ~IDestructible();
};

__declspec(weak) IDestructible::~IDestructible() {
}
