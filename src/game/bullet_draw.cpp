// A fragment of bullet.cpp (0x800ccb7c): CBullet::IsDrawEnabled (true). The classes are named by the mangled symbols
// and are non-virtual views; members and result types are inferred. The
// functions are weak copies of header inlines emitted in this file, so they
// are defined __declspec(weak). The rest of the file is not part of this
// unit.
class CBullet {
public:
    bool IsDrawEnabled() const;
};

__declspec(weak) bool CBullet::IsDrawEnabled() const {
    return true;
}
