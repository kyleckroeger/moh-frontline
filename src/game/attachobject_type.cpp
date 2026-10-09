// First in this unit (0x800971f8): the weak CAttachableObject destructor (its
// table pointer, then CStaticObject's destructor, called; the object freed
// when asked). The classes are views: CAttachableObject declares
// MarkForDestruction (defined elsewhere) first so its global table stays
// elsewhere.
// A fragment of attachobject.cpp (0x80097258): CAttachableObject::GetObjectType (1). The classes are named by the mangled symbols
// and are non-virtual views; members and result types are inferred. The
// functions are weak copies of header inlines emitted in this file, so they
// are defined __declspec(weak). The rest of the file is not part of this
// unit.
class CStaticObject {
public:
    virtual ~CStaticObject();
};

class CAttachableObject : public CStaticObject {
public:
    virtual void MarkForDestruction(int); /* defined elsewhere; keeps the table out */
    virtual ~CAttachableObject();
    int GetObjectType() const;
};

__declspec(weak) CAttachableObject::~CAttachableObject() {
}

__declspec(weak) int CAttachableObject::GetObjectType() const {
    return 1;
}
