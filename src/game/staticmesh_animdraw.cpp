// A fragment of staticmesh.cpp (0x800f15e4): CStaticAnimMesh::Draw (the
// frame's mesh is made current, the static mesh drawn and the first frame's
// mesh restored). The file name is this project's; the original record is
// staticmesh.cpp. The classes are named by the mangled symbols; the animated
// mesh data and the members are inferred. CStaticMesh declares Draw and
// CStaticAnimMesh its destructor (both defined elsewhere) first so no table
// is emitted here.
class CDrawContext;
struct StaticMesh;

/* Inferred: the animated mesh data's mesh table at +8 and frame count at +12. */
struct SAnimMeshData {
    unsigned char unknown00[8];
    StaticMesh** m_meshes;
    int m_count;
};

class CStaticMesh {
public:
    virtual void Draw(CDrawContext&, int);
    virtual ~CStaticMesh();

    StaticMesh* m_mesh;
};

class CStaticAnimMesh : public CStaticMesh {
public:
    virtual ~CStaticAnimMesh();
    virtual void Draw(CDrawContext&, int);

    unsigned char unknown08[4];
    SAnimMeshData* m_data;
};

void CStaticAnimMesh::Draw(CDrawContext& context, int frame) {
    m_mesh = m_data->m_meshes[frame];
    CStaticMesh::Draw(context, frame);
    m_mesh = m_data->m_meshes[0];
}
