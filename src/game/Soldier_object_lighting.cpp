// A fragment of Soldier_object.cpp (0x800aa1a8): CStaticMesh::EnableLighting (the top flag bit at +8). The classes are named by the mangled symbols
// and are non-virtual views; members and result types are inferred. The
// functions are weak copies of header inlines emitted in this file, so they
// are defined __declspec(weak). The rest of the file is not part of this
// unit.
class CStaticMesh {
public:
    void EnableLighting(bool);

    unsigned char unknown0[8];
    bool m_lighting : 1;
    bool m_fogging : 1;
    unsigned char m_flags : 6;
};

__declspec(weak) void CStaticMesh::EnableLighting(bool enable) {
    m_lighting = enable;
}
