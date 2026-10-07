// A fragment of hearingvolume.cpp (0x80098948): the empty CBullet::Penetrate and SetExclusionPair. The classes are named by the mangled symbols
// and are non-virtual views; members and result types are inferred. The
// functions are weak copies of header inlines emitted in this file, so they
// are defined __declspec(weak). The rest of the file is not part of this
// unit.
class CCollision;
class ISceneNode;

class CBullet {
public:
    void Penetrate(const CCollision&);
    void SetExclusionPair(ISceneNode*);
};

__declspec(weak) void CBullet::Penetrate(const CCollision&) {
}

__declspec(weak) void CBullet::SetExclusionPair(ISceneNode*) {
}
