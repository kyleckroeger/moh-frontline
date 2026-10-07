// A fragment of staticmesh.cpp (0x800f2510): CStaticMesh's destructor and
// constructor (records the mesh data, sets two flag bits and initialises the
// mesh when asked), whose definitions place the class's virtual table here.
// CStaticMesh, StaticMesh and the virtual functions are named by the mangled
// symbols and the virtual table; the members and flag bits are inferred.
// Built with -RTTI on: the virtual table's second word points at the
// __RTTI__11CStaticMesh record in .sdata.
class CDrawContext;
struct StaticMesh;

class CStaticMesh {
public:
    CStaticMesh(StaticMesh*, bool);
    virtual ~CStaticMesh();
    virtual void Draw(CDrawContext&, int);
    virtual void EnableLighting(bool);
    virtual bool IsLightingEnabled();
    virtual void EnableFogging(bool);
    virtual bool IsFoggingEnabled();
    void Init();

    StaticMesh* m_mesh;
    unsigned char m_flag80 : 1;
    unsigned char m_flag40 : 1;
    unsigned char m_flags : 6;
};

CStaticMesh::~CStaticMesh() {
}

CStaticMesh::CStaticMesh(StaticMesh* mesh, bool init) {
    m_mesh = mesh;
    m_flag80 = 1;
    m_flag40 = 1;
    if (init)
        Init();
}
