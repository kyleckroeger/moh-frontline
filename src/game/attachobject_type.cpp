// A fragment of attachobject.cpp (0x80097258): CAttachableObject::GetObjectType (1). The classes are named by the mangled symbols
// and are non-virtual views; members and result types are inferred. The
// functions are weak copies of header inlines emitted in this file, so they
// are defined __declspec(weak). The rest of the file is not part of this
// unit.
class CAttachableObject {
public:
    int GetObjectType() const;
};

__declspec(weak) int CAttachableObject::GetObjectType() const {
    return 1;
}
