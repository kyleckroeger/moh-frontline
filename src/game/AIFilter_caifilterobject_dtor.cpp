// A fragment of AIFilter.cpp (0x8005be5c): the CAIFilterObject destructor (its virtual
// table pointer, then the object freed when asked). The file name is this
// project's; the original record is AIFilter.cpp. The class and the destructor
// are named by the mangled symbols, and only the virtuals the destructor
// needs are declared. CAIFilterObject declares another of its virtual functions
// (defined elsewhere; the result type is not known) before its destructor,
// so its global virtual table is not emitted here.
class CAIFilterObject {
public:
    virtual void ChooseTarget(float); /* result type not known */
    virtual ~CAIFilterObject();
};

CAIFilterObject::~CAIFilterObject() {
}
