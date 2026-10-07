// A fragment of staticobjfactory.cpp (0x800b327c): CStaticMesh::EnableFogging (the second flag bit at +8) and the StaticObjectTemplate constructor (first word -1). The classes are named by the mangled symbols
// and are non-virtual views; members and result types are inferred. The
// functions are weak copies of header inlines emitted in this file, so they
// are defined __declspec(weak). The rest of the file is not part of this
// unit.
class CStaticMesh {
public:
    void EnableFogging(bool);

    unsigned char unknown0[8];
    bool m_lighting : 1;
    bool m_fogging : 1;
    unsigned char m_flags : 6;
};

struct StaticObjectTemplate {
    StaticObjectTemplate();

    int m_id;
};

__declspec(weak) void CStaticMesh::EnableFogging(bool enable) {
    m_fogging = enable;
}

__declspec(weak) StaticObjectTemplate::StaticObjectTemplate() {
    m_id = -1;
}
