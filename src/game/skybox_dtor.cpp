// A fragment of skybox.cpp (0x800a918c): the CSkyBox destructor (its virtual
// table pointer; the six files at +424 closed; then the inline ISceneNode
// destructor and IObserver's; the object freed when asked). The file name is
// this project's; the original record is skybox.cpp. The classes and
// functions are named by the mangled symbols; the members and their names are
// inferred, and only the virtuals the destructor needs are declared. CSkyBox
// declares Draw (defined elsewhere; the result type is not known) first so
// its global virtual table is not emitted here. ISceneNode's destructor is
// inline and its table weak in the original, so the compiler's copies are
// weak duplicates, linked to the original copies.
class CDrawContext;
void TLT_CloseFile(void*);

class IDestructible {
public:
    IDestructible() {}
    virtual void MarkForDestruction(int);
    virtual ~IDestructible();
};

class ISubject : public IDestructible {
public:
    ISubject() {}
    virtual ~ISubject();
};

class IObserver : public ISubject {
public:
    IObserver() {}
    virtual ~IObserver();
};

class ISceneNode : public IObserver {
public:
    ISceneNode() : data04(0), data08(0), data0c(0), data10(0), data14(0), data18(0), data1c(0), data20(0) {}
    virtual ~ISceneNode() {}

    int data04;
    int data08;
    int data0c;
    int data10;
    int data14;
    int data18;
    int data1c;
    int data20;
};

class CSkyBox : public ISceneNode {
public:
    virtual void Draw(CDrawContext&); /* result type not known */
    virtual ~CSkyBox();

    unsigned char unknown24[388];
    void* m_files[6];
};

CSkyBox::~CSkyBox() {
    for (int i = 0; i < 6; i++)
        TLT_CloseFile(m_files[i]);
}
