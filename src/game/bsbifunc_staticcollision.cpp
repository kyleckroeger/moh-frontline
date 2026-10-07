// A fragment of bsbifunc.cpp (0x8002ecd8): the weak
// CStaticObject::SetCollisionId (the id at +476), a weak copy emitted between
// the script built-ins. The file name is this project's; the original record is
// bsbifunc.cpp and the functions around it are not reconstructed. The function
// and class are named by the mangled symbol; CStaticObject is a non-virtual
// view with the id at its offset (the function is virtual in
// __vt__13CStaticObject).
enum EClsnId {};

class CStaticObject {
public:
    void SetCollisionId(EClsnId);

    unsigned char unknown000[476];
    EClsnId m_collisionId;
};

__declspec(weak) void CStaticObject::SetCollisionId(EClsnId id) {
    m_collisionId = id;
}
