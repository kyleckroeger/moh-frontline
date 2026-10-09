// A fragment of AIPlayerDoodad.cpp (0x80059298): the CAIPlayerDoodad
// destructor (its table pointer, then CAIDoodad's destructor) and
// constructor (CAIDoodad's constructor, its table pointer, the filter pointer
// at +4 cleared). The file name is this project's; the original record is
// AIPlayerDoodad.cpp. The classes are named by the mangled symbols; the
// member is inferred. CAIPlayerDoodad declares GetSceneNode (defined
// elsewhere) first so that its table stays elsewhere.
class ISceneNode;
class CAIFilterPlayerObject;

class CAIDoodad {
public:
    CAIDoodad();
    virtual ~CAIDoodad();
};

class CAIPlayerDoodad : public CAIDoodad {
public:
    virtual ISceneNode* GetSceneNode() const;
    virtual ~CAIPlayerDoodad();
    CAIPlayerDoodad();

    CAIFilterPlayerObject* m_filter;
};

CAIPlayerDoodad::~CAIPlayerDoodad() {
}

CAIPlayerDoodad::CAIPlayerDoodad() {
    m_filter = 0;
}
