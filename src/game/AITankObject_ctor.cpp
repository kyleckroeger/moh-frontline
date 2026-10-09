// A fragment of AITankObject.cpp (0x8005230c): the CAITankObject placement
// operator new, destructor (its table pointer, then CAIObject's destructor;
// never freed, as the class's operator delete does nothing) and constructor
// (CAIObject's constructor, its table pointer, three positions at +1912
// built through their constructor, the owner kept at +8, target mode 3),
// then the weak, empty CAIFilterRealPosition constructor emitted after it.
// The file name is this project's; the original record is AITankObject.cpp.
// The classes and functions are named by the mangled symbols; the members and
// the empty operator delete are inferred. CAITankObject declares PreUpdate
// (defined elsewhere) first so that its table stays elsewhere.
enum aifilter_target_mode {};

class CAIFilterRealPosition {
public:
    CAIFilterRealPosition() {}

    float x;
    float y;
    float z;
    float w;
};

class CAIObject {
public:
    CAIObject();
    virtual ~CAIObject();

    unsigned char unknown0004[4];
    void* m_owner;
    unsigned char unknown000c[1836];
};

class CAITankObject : public CAIObject {
public:
    virtual void PreUpdate();
    static void* operator new(unsigned long, void*);
    static void operator delete(void*) {}
    virtual ~CAITankObject();
    CAITankObject(void*);

    unsigned char unknown0738[8];
    aifilter_target_mode m_targetMode;
    unsigned char unknown0744[52];
    CAIFilterRealPosition m_positions[3];
};

void* CAITankObject::operator new(unsigned long, void* place) {
    return place;
}

CAITankObject::~CAITankObject() {
}

CAITankObject::CAITankObject(void* owner) {
    m_owner = owner;
    m_targetMode = (aifilter_target_mode)3;
}
