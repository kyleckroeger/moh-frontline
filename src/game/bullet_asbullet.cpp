// A fragment of bullet.cpp (0x800ccbf8): both CBullet::AsBullet (itself). The classes are named by the mangled symbols
// and are non-virtual views; members and result types are inferred. The
// functions are weak copies of header inlines emitted in this file, so they
// are defined __declspec(weak). The rest of the file is not part of this
// unit.
class CBullet {
public:
    const CBullet* AsBullet() const;
    CBullet* AsBullet();
};

__declspec(weak) const CBullet* CBullet::AsBullet() const {
    return this;
}

__declspec(weak) CBullet* CBullet::AsBullet() {
    return this;
}
