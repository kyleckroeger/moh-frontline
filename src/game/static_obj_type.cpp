// A fragment of static_obj.cpp (0x800ae804): CStaticObject::GetObjectType (0). The classes are named by the mangled symbols
// and are non-virtual views; members and result types are inferred. The
// functions are weak copies of header inlines emitted in this file, so they
// are defined __declspec(weak). The rest of the file is not part of this
// unit.
class CStaticObject {
public:
    int GetObjectType() const;
};

__declspec(weak) int CStaticObject::GetObjectType() const {
    return 0;
}
