// A fragment of staticmesh.cpp (0x800f1634): the CStaticAnimMesh destructor (its virtual table
// pointer then each base's through the
// inline destructors of CStaticMesh, and the object freed when asked). The file
// name is this project's; the original record is staticmesh.cpp. The classes and
// the destructor are named by the mangled symbols; the layout before the
// table pointer is not known, and only the virtuals the destructor needs are
// declared. CStaticAnimMesh declares another of its virtual functions (defined
// elsewhere; the result type is not known) before its destructor, so its own
// global virtual table is not emitted here.
// The destructor of CStaticMesh is global in the original (out of line,
// elsewhere in the same file) yet inlined here; how the original made it available
// inline is not known, so the view declares it inline, after another of
// the class's own virtual functions so that its global table is not emitted.
class CDrawContext;

class CStaticMesh {
public:
    virtual void Draw(CDrawContext&, int); /* result type not known */
    virtual ~CStaticMesh() {}
};

class CStaticAnimMesh : public CStaticMesh {
public:
    virtual void Draw(CDrawContext&, int); /* result type not known */
    virtual ~CStaticAnimMesh();
};

CStaticAnimMesh::~CStaticAnimMesh() {
}
