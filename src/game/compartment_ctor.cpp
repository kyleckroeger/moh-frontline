// A fragment of compartment.cpp (0x80076030): the CCompartment copy
// constructor and default constructor. Both run the scene-node base
// constructors (table pointers, eight cleared words) and set the
// compartment's table pointer; the copy then copies the collision object at
// +40 (CCDBObject's copy constructor), four words and a byte and clears the
// byte at +97, and the default one builds the collision object from no
// database and sets -1, three cleared words, 1 and 0. The file name is this
// project's; the original record is compartment.cpp. The classes and
// functions are named by the mangled symbols; the members and their names
// are inferred views, and only the virtuals these constructors need are
// declared. CCompartment declares one of its own virtual functions (defined
// elsewhere; the result type is not known) first so its global virtual table
// is not emitted here. ISceneNode's destructor is inline and its table weak
// in the original, so the compiler's copies are weak duplicates, linked to
// the original copies.
class CDB;
class CDrawContext;
class CCollision;

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

/* inferred: a 16-byte vector copied as two doubles (as in cdbobject_ctor.cpp) */
struct CVector3View {
    double pair[2];
};

class IVolume {
public:
    virtual ~IVolume();
};

class CCDBObject : public IVolume {
public:
    virtual bool TestCollision(const IVolume&, CCollision&, bool) const;
    virtual ~CCDBObject();
    CCDBObject(const CCDBObject&);
    CCDBObject(CDB*);

    CDB* m_db;
    CVector3View data08;
    CVector3View data18;
};

class CCompartment : public ISceneNode {
public:
    virtual void Draw(CDrawContext&); /* result type not known */
    virtual ~CCompartment();
    CCompartment(const CCompartment&);
    CCompartment();

    unsigned char unknown24[4];
    CCDBObject m_cdb;
    int data50;
    int data54;
    int data58;
    int data5c;
    bool data60;
    bool data61;
};

CCompartment::CCompartment(const CCompartment& other)
    : m_cdb(other.m_cdb), data50(other.data50), data54(other.data54), data58(other.data58), data5c(other.data5c),
      data60(other.data60), data61(false) {
}

CCompartment::CCompartment()
    : m_cdb(0), data50(-1), data54(0), data58(0), data5c(0), data60(true), data61(false) {
}
