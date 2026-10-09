// A fragment of AISoldierDoodad.cpp (0x800596c4): the CAISoldierDoodad
// destructor (its table pointer; the filter object at +4 deleted through its
// virtual destructor; then CAIDoodad's destructor) and constructor
// (CAIDoodad's constructor, then its table pointer). The file name is this
// project's; the original record is AISoldierDoodad.cpp. The classes are
// named by the mangled symbols; the member is inferred. CAISoldierDoodad
// declares GetSceneNode (defined elsewhere) first so that its table stays
// elsewhere.
class ISceneNode;

class CAIFilterSoldierObject {
public:
    virtual ~CAIFilterSoldierObject();
};

class CAIDoodad {
public:
    CAIDoodad();
    virtual ~CAIDoodad();
};

class CAISoldierDoodad : public CAIDoodad {
public:
    virtual ISceneNode* GetSceneNode() const;
    virtual ~CAISoldierDoodad();
    CAISoldierDoodad();

    CAIFilterSoldierObject* m_filter;
};

CAISoldierDoodad::~CAISoldierDoodad() {
    delete m_filter;
}

CAISoldierDoodad::CAISoldierDoodad() {
}
