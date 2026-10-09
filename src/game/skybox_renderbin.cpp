// A fragment of skybox.cpp (0x800a975c): the weak CSkyBoxRenderBin destructor
// (its virtual table pointer, then CRenderBin's inline destructor, and the
// object freed when asked) and weak default constructor (CRenderBinData's
// inline constructor, the table pointers, two words and a flag cleared, then
// the priority set to 1 and the bin's Init called through its table). The file
// name is this project's; the original record is skybox.cpp. The classes and
// functions are named by the mangled symbols (CRenderBinData by CRenderBin's
// RTTI record); the members are inferred. CSkyBoxRenderBin's own virtuals
// (Render, Link, IsUsed) are weak in the original too; their bodies are not
// part of this view, so they are declared without one, which keeps its table
// out of this unit. CRenderBin's weak destructor and default Render/Link
// copies the compiler emits here are weak duplicates, linked to the original
// copies.
class CDmaTag;
class CDmaPacket;

class CRenderBin;

/* CRenderBin's non-polymorphic base, named by CRenderBin's RTTI record; the
   members are inferred (CRenderBin::Init walks children from +8 through the
   siblings at +4, IsUsed tests +16, +20 is the render priority, 3 by
   default). */
struct CRenderBinData {
    CRenderBinData() : data00(0), m_next(0), m_children(0), data0c(0), data10(0), m_priority(3) {}

    int data00;
    CRenderBin* m_next;
    CRenderBin* m_children;
    int data0c;
    int data10;
    int m_priority;
    int data18;
};

/* CRenderBin's virtuals in table order; all are weak (inline) in the
   original. Init's and IsUsed's bodies are not part of this view, so they
   are declared without one, which also keeps CRenderBin's table out of this
   unit. */
class CRenderBin : public CRenderBinData {
public:
    CRenderBin() {}
    virtual ~CRenderBin() {}
    virtual int Render(CDmaPacket&, void*) { return 0; }
    virtual void Init();
    virtual void Link(CDmaTag*, CDmaPacket&) {}
    virtual bool IsUsed();
};

class CSkyBoxRenderBin : public CRenderBin {
public:
    virtual int Render(CDmaPacket&, void*);
    CSkyBoxRenderBin();
    virtual ~CSkyBoxRenderBin();
    virtual void Link(CDmaTag*, CDmaPacket&);
    virtual bool IsUsed();

    int data20;
    int data24;
    bool data28;
};

__declspec(weak) CSkyBoxRenderBin::~CSkyBoxRenderBin() {
}

__declspec(weak) CSkyBoxRenderBin::CSkyBoxRenderBin() : data20(0), data24(0), data28(false) {
    m_priority = 1;
    Init();
}
