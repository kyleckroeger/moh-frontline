// CTankObject's weak AI-doodad accessors and cast, emitted at the start of the
// file. The names come from the mangled symbols; the doodad's offset is
// inferred, the result types are inferred, and the class is a non-virtual
// view. They are inline in the original (weak symbols), so they are defined
// __declspec(weak). FireSubObject and the rest of the file are not part of
// this unit.
class CAIDoodad;
class CHierTankObject;

class CTankObject {
public:
    const CAIDoodad* GetAIDoodad() const;
    CAIDoodad* GetAIDoodad();
    CHierTankObject* AsHierTankObject();

    unsigned char unknown000[1856];
    unsigned char m_aiDoodad[4];
};

__declspec(weak) const CAIDoodad* CTankObject::GetAIDoodad() const {
    return (const CAIDoodad*)m_aiDoodad;
}

__declspec(weak) CAIDoodad* CTankObject::GetAIDoodad() {
    return (CAIDoodad*)m_aiDoodad;
}

__declspec(weak) CHierTankObject* CTankObject::AsHierTankObject() {
    return (CHierTankObject*)this;
}
