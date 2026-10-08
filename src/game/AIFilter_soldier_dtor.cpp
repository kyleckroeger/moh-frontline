// A fragment of AIFilter.cpp (0x8005bd34): the CAIFilterSoldierObject destructor (its virtual
// table pointer, the AI object at +8 deleted through its virtual destructor,
// then CAIFilterObject's table pointer). The filter has no delete call: it is
// constructed in place in its owner's memory (see AITankDoodad.cpp), so its
// operator delete is an inferred empty inline. The file name is this
// project's; the original record is AIFilter.cpp. The classes and functions
// are named by the mangled symbols; the members follow AIFilter.cpp's view (whose first word is the table pointer),
// and only the virtuals the destructor needs are declared. CAIFilterObject's
// destructor is defined later in the same original file
// (AIFilter_caifilterobject_dtor.cpp) and inlined here; this fragment cannot
// hold it, so the view declares it inline. Each class declares ChooseTarget
// (defined elsewhere; the result type is not known) before its destructor so
// the global virtual tables are not emitted here.
class CAIObject {
public:
    virtual ~CAIObject();
};

class CAIFilterObject {
public:
    virtual void ChooseTarget(float); /* result type not known */
    virtual ~CAIFilterObject() {}

    void* m_gameObject;
    CAIObject* m_aiObject;
};

class CAIFilterSoldierObject : public CAIFilterObject {
public:
    virtual void ChooseTarget(float); /* result type not known */
    virtual ~CAIFilterSoldierObject();

    static void operator delete(void*) {}
};

CAIFilterSoldierObject::~CAIFilterSoldierObject() {
    delete m_aiObject;
}
