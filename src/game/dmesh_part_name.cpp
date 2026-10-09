// A fragment of dmesh.cpp (0x800f6a48): the weak CPart::GetName ("DMESH Part"). The
// file name is this project's; the original record is dmesh.cpp. CPart and
// GetName are named by the mangled symbols; the result type is inferred. The
// string links to its pool entry. CDMesh::InitClass before it addresses the
// file's .bss block through one base register and is not part of this unit.
class CPart {
public:
    const char* GetName();
};

__declspec(weak) const char* CPart::GetName() {
    return "DMESH Part";
}
