// A fragment of AIPlayerObject.cpp (0x8004f270): the CAIPlayerObject
// destructor (its table pointer, then CAIObject's destructor, and the object
// freed when asked) and constructor (CAIObject's constructor, its table
// pointer, the owner kept at +8). The file name is this project's; the
// original record is AIPlayerObject.cpp. The classes are named by the mangled
// symbols; the member is inferred. CAIPlayerObject declares PreUpdate
// (defined elsewhere) first so that its table stays elsewhere.
class CAIObject {
public:
    CAIObject();
    virtual ~CAIObject();

    unsigned char unknown04[4];
    void* m_owner;
};

class CAIPlayerObject : public CAIObject {
public:
    virtual void PreUpdate();
    virtual ~CAIPlayerObject();
    CAIPlayerObject(void*);
};

CAIPlayerObject::~CAIPlayerObject() {
}

CAIPlayerObject::CAIPlayerObject(void* owner) {
    m_owner = owner;
}
