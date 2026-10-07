// A fragment of static_obj.cpp (0x800af258): CThrownBullet::GetFiredBy (the pointer at +172). The classes are named by the mangled symbols
// and are non-virtual views; members and result types are inferred. The
// functions are weak copies of header inlines emitted in this file, so they
// are defined __declspec(weak). The rest of the file is not part of this
// unit.
class ISceneNode;

class CThrownBullet {
public:
    ISceneNode* GetFiredBy() const;

    unsigned char unknown000[172];
    ISceneNode* m_firedBy;
};

__declspec(weak) ISceneNode* CThrownBullet::GetFiredBy() const {
    return m_firedBy;
}
