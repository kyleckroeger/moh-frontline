// A fragment of thrownbullet.cpp (0x800d1c58): CThrownBullet::Init (the bullet's Init). The classes are named by the mangled symbols
// and are non-virtual views; members and result types are inferred. The
// functions are weak copies of header inlines emitted in this file, so they
// are defined __declspec(weak). The rest of the file is not part of this
// unit.
class CBullet {
public:
    void Init();
};

class CThrownBullet : public CBullet {
public:
    void Init();
};

__declspec(weak) void CThrownBullet::Init() {
    CBullet::Init();
}
