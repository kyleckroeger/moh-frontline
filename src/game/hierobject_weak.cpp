// A fragment of hierobject.cpp (0x80098de4): weak copies of header inlines
// emitted in this file: CHierObject::Init (the static object's Init), its
// object type (8) and AsHierObject (itself). CHierObject and CStaticObject are
// named by the mangled symbols and are non-virtual views; the result types
// are inferred. The rest of the file is not part of this unit.
class CStaticObject {
public:
    void Init();
};

class CHierObject : public CStaticObject {
public:
    void Init();
    int GetObjectType() const;
    const CHierObject* AsHierObject() const;
    CHierObject* AsHierObject();
};

__declspec(weak) void CHierObject::Init() {
    CStaticObject::Init();
}

__declspec(weak) int CHierObject::GetObjectType() const {
    return 8;
}

__declspec(weak) const CHierObject* CHierObject::AsHierObject() const {
    return this;
}

__declspec(weak) CHierObject* CHierObject::AsHierObject() {
    return this;
}
