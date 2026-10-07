// A fragment of AIObject.cpp (0x800478a8): the weak CAIObject defaults
// IsCrouching and IsPlayer (both false), copies of header inlines emitted in
// this file. CAIObject is named by the mangled symbols and is a non-virtual
// view; the result types are inferred. The rest of the file is not part of
// this unit.
class CAIObject {
public:
    bool IsCrouching() const;
    bool IsPlayer() const;
};

__declspec(weak) bool CAIObject::IsCrouching() const {
    return false;
}

__declspec(weak) bool CAIObject::IsPlayer() const {
    return false;
}
