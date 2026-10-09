// CAIDoodad, the whole of AIDoodad.cpp: the weak default scene node (none), an
// empty Init, the destructor (its table pointer reset, the object freed when
// asked) and the constructor (its table pointer, +4 cleared and four
// pointers into the doodad's own storage at +32, +44, +56 and +68). The
// destructor is the class's key function, so its table and type information
// are emitted here (built with RTTI on). CAIDoodad, CStaticObject and the
// functions are named by the mangled symbols; the members are inferred. The
// inline fire methods in the table are emitted elsewhere and are declared
// without bodies.
class ISceneNode;
class CStaticObject;

class CAIDoodad {
public:
    virtual ~CAIDoodad();
    virtual ISceneNode* GetSceneNode() const;
    virtual void FireAtCurrentTarget(float); /* result type not known */
    virtual void FireMMGAtCurrentTarget(CStaticObject*); /* result type not known */
    CAIDoodad();
    void Init();

    int data04;
    unsigned char unknown08[8];
    void* data10;
    void* data14;
    void* data18;
    void* data1c;
    unsigned char m_storage0[12];
    unsigned char m_storage1[12];
    unsigned char m_storage2[12];
    unsigned char m_storage3[12];
};

__declspec(weak) ISceneNode* CAIDoodad::GetSceneNode() const {
    return 0;
}

void CAIDoodad::Init() {
}

CAIDoodad::~CAIDoodad() {
}

CAIDoodad::CAIDoodad() {
    data04 = 0;
    data10 = m_storage0;
    data14 = m_storage1;
    data18 = m_storage2;
    data1c = m_storage3;
}
