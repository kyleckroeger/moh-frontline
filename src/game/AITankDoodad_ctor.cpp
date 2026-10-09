// A fragment of AITankDoodad.cpp (0x800598ac): the CAITankDoodad destructor
// (its table pointer; the filter object at +4 deleted through its virtual
// destructor and cleared; then CAIDoodad's destructor) and constructor
// (CAIDoodad's constructor, then its table pointer). The file name is this
// project's; the original record is AITankDoodad.cpp. The classes are named by
// the mangled symbols; the member is inferred. CAITankDoodad declares
// GetSceneNode (defined elsewhere) first so that its table stays elsewhere.
class ISceneNode;

class CAIFilterTankObject {
public:
    virtual ~CAIFilterTankObject();
};

class CAIDoodad {
public:
    CAIDoodad();
    virtual ~CAIDoodad();
};

class CAITankDoodad : public CAIDoodad {
public:
    virtual ISceneNode* GetSceneNode() const;
    virtual ~CAITankDoodad();
    CAITankDoodad();

    CAIFilterTankObject* m_filter;
};

CAITankDoodad::~CAITankDoodad() {
    delete m_filter;
    m_filter = 0;
}

CAITankDoodad::CAITankDoodad() {
}
