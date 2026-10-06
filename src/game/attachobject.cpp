// CStaticObject's weak casts, draw-enabled flag and attached light, emitted at
// the start of this file. CStaticObject and CLight are named by the mangled
// symbols; the flag byte is an inferred bit-field view and the members are
// inferred from offsets. The class is a non-virtual view; the accessors are
// inline in the original (weak symbols), so they are defined __declspec(weak).
// The rest of the file is not part of this unit.
class CLight;

class CStaticObject {
public:
    const CStaticObject* AsStaticObject() const;
    CStaticObject* AsStaticObject();
    bool IsDrawEnabled() const;
    CLight* GetAttachedLight() const;

    unsigned char unknown000[480];
    bool drawEnabled : 1;
    unsigned char flags : 7;
    unsigned char unknown1e1[107];
    CLight* m_attachedLight;
};

__declspec(weak) const CStaticObject* CStaticObject::AsStaticObject() const {
    return this;
}

__declspec(weak) CStaticObject* CStaticObject::AsStaticObject() {
    return this;
}

__declspec(weak) bool CStaticObject::IsDrawEnabled() const {
    return drawEnabled;
}

__declspec(weak) CLight* CStaticObject::GetAttachedLight() const {
    return m_attachedLight;
}
