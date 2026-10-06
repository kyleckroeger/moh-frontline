// CPlayerObject's weak AI-doodad accessor and identity casts. The names come
// from the mangled symbols; the doodad's offset and the result types are
// inferred, and the class is a non-virtual view. They are inline in the
// original (weak symbols), so they are defined __declspec(weak). The rest of
// the file is not part of this unit.
class CAIDoodad;

class CPlayerObject {
public:
    const CAIDoodad* GetAIDoodad() const;
    const CPlayerObject* AsPlayerObject() const;
    CPlayerObject* AsPlayerObject();

    unsigned char unknown00[36];
    unsigned char m_aiDoodad[4];
};

__declspec(weak) const CAIDoodad* CPlayerObject::GetAIDoodad() const {
    return (const CAIDoodad*)m_aiDoodad;
}

__declspec(weak) const CPlayerObject* CPlayerObject::AsPlayerObject() const {
    return this;
}

__declspec(weak) CPlayerObject* CPlayerObject::AsPlayerObject() {
    return this;
}
