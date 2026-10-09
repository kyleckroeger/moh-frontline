// A fragment of AISoldierObject.cpp (0x8004f9d8): the CAISoldierObject
// destructor (its table pointer, then CAIObject's destructor; the object is
// never freed, as the class's operator delete does nothing), the constructor
// (CAIObject's constructor, its table pointer, a vector at +1880 cleared, the
// owner kept at +8, target mode 3 and the matching target list) and the
// placement operator new. The file name is this project's; the original
// record is AISoldierObject.cpp. The classes and functions are named by the
// mangled symbols; the members, the empty operator delete and the 0.0
// constants (entries of the file's .sdata2 pool) are inferred.
// CAISoldierObject declares PreUpdate (defined elsewhere) first so that its
// table stays elsewhere.
enum aifilter_target_mode {};

class CAIObject {
public:
    CAIObject();
    virtual ~CAIObject();

    unsigned char unknown0004[4];
    void* m_owner;
    unsigned char unknown000c[1868];
    float data758;
    float data75c;
    float data760;
    unsigned char unknown0764[12];
};

class CAISoldierObject : public CAIObject {
public:
    virtual void PreUpdate();
    virtual ~CAISoldierObject();
    CAISoldierObject(void*);
    static void* operator new(unsigned long, void*);
    static void operator delete(void*) {}
    void ResetTargetMatchList(aifilter_target_mode);

    aifilter_target_mode m_targetMode;
};

CAISoldierObject::~CAISoldierObject() {
}

CAISoldierObject::CAISoldierObject(void* owner) {
    data758 = 0.0f;
    data75c = 0.0f;
    data760 = 0.0f;
    m_owner = owner;
    m_targetMode = (aifilter_target_mode)3;
    ResetTargetMatchList((aifilter_target_mode)3);
}

void* CAISoldierObject::operator new(unsigned long, void* place) {
    return place;
}
