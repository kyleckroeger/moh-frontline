// CThrownObject: the weak object-type, cast and script-transform defaults and
// the deleted flag. The class and enum names come from the mangled symbols;
// the flag byte is an inferred bit-field view, the result types are inferred,
// and the class is a non-virtual view. The defaults are inline in the
// original (weak symbols), so they are defined __declspec(weak). Destroy and
// the rest of the file are not part of this unit.
class CVector3;
enum EBSEventEnum {};

class CThrownObject {
public:
    int GetObjectType() const;
    CThrownObject* AsThrownObject();
    void TranslateFromScript(CVector3&, CVector3&, EBSEventEnum);
    void RotateFromScript(CVector3&, CVector3&, EBSEventEnum);
    void ScaleFromScript(float, float, EBSEventEnum);
    void SetDeleted(bool);

    unsigned char unknown000[756];
    unsigned char flag0 : 1;
    unsigned char flag1 : 1;
    unsigned char flag2 : 1;
    unsigned char flag3 : 1;
    unsigned char deleted : 1;
    unsigned char flag5 : 1;
    unsigned char flag6 : 1;
    unsigned char flag7 : 1;
};

__declspec(weak) int CThrownObject::GetObjectType() const {
    return 4;
}

__declspec(weak) CThrownObject* CThrownObject::AsThrownObject() {
    return this;
}

__declspec(weak) void CThrownObject::TranslateFromScript(CVector3&, CVector3&, EBSEventEnum) {
}

__declspec(weak) void CThrownObject::RotateFromScript(CVector3&, CVector3&, EBSEventEnum) {
}

__declspec(weak) void CThrownObject::ScaleFromScript(float, float, EBSEventEnum) {
}

void CThrownObject::SetDeleted(bool deletedFlag) {
    deleted = deletedFlag;
}
