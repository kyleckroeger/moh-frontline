// A fragment of bsbifunc.cpp (0x8002fcec): the weak
// CAnimObject::GetScriptObject (the word at +9104), a weak copy emitted between
// the script built-ins. The file name is this project's; the original record is
// bsbifunc.cpp and the functions around it are not reconstructed. The function
// and class are named by the mangled symbol; CAnimObject is a non-virtual view
// with the word at its offset (the function is virtual in the class's table;
// its return type is not visible here).
class CAnimObject {
public:
    int GetScriptObject() const;

    unsigned char unknown0000[9104];
    int m_scriptObject;
};

__declspec(weak) int CAnimObject::GetScriptObject() const {
    return m_scriptObject;
}
