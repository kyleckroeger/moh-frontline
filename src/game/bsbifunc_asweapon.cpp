// A fragment of bsbifunc.cpp (0x80025874): the weak
// CStaticObject::AsWeaponObject (0), a weak copy emitted between the script
// built-ins. The file name is this project's; the original record is
// bsbifunc.cpp and the functions around it are not reconstructed. The function
// and class are named by the mangled symbol; CStaticObject is a non-virtual
// view (the function is virtual in __vt__13CStaticObject; its return type is
// not visible here).
class CStaticObject {
public:
    void* AsWeaponObject();
};

__declspec(weak) void* CStaticObject::AsWeaponObject() {
    return 0;
}
