// A fragment of Moh2.cpp (0x8001ae94): weak default functions of
// IMovingSceneNode (no light volume handling, no light volume, itself as the
// moving node) and ISceneNode (empty update, AI, collision and draw hooks, no
// bounding volumes, the identity as its transform), copies of header inlines
// emitted in this file (so they are defined __declspec(weak)). The classes
// are named by the mangled symbols; IMovingSceneNode and ISceneNode are
// non-virtual views. GetPosition (the origin) and the unit direction getters
// follow; their constants are entries of the file's .sdata2 pool, and the
// CVector3 view and its three-float setter are inferred. The rest of the file
// is not part of this unit.
struct BPDLightVolume;
class CCollision;
class CDrawContext;
class IVolume;

class CVector3 {
public:
    void Set(float x_, float y_, float z_) {
        x = x_;
        y = y_;
        z = z_;
    }

    float x;
    float y;
    float z;
} __attribute__((aligned(8)));

class CMatrix {
public:
    void Ident();
};

class IMovingSceneNode {
public:
    void EnterLightVolume(BPDLightVolume*);
    void ExitLightVolume(BPDLightVolume*);
    BPDLightVolume* GetLightVolume();
    const IMovingSceneNode* AsMovingNode() const;
    IMovingSceneNode* AsMovingNode();
};

class ISceneNode {
public:
    enum EVolumeType {};

    void BeginUpdate(float);
    void UpdateAI(float);
    void CommitAI();
    void ConstrainVelocity();
    void AttemptUpdate(float);
    void OnCollision(const CCollision&);
    void CommitUpdate();
    void Draw(CDrawContext&);
    IVolume* GetLocalBoundingVolume(EVolumeType) const;
    IVolume* GetWorldBoundingVolume(EVolumeType) const;
    void GetTMLocalToWorld(CMatrix&) const;
    void GetPosition(CVector3&) const;
    void GetRightward(CVector3&) const;
    void GetForward(CVector3&) const;
    void GetUpward(CVector3&) const;
};

__declspec(weak) void IMovingSceneNode::EnterLightVolume(BPDLightVolume*) {
}

__declspec(weak) void IMovingSceneNode::ExitLightVolume(BPDLightVolume*) {
}

__declspec(weak) BPDLightVolume* IMovingSceneNode::GetLightVolume() {
    return 0;
}

__declspec(weak) const IMovingSceneNode* IMovingSceneNode::AsMovingNode() const {
    return this;
}

__declspec(weak) IMovingSceneNode* IMovingSceneNode::AsMovingNode() {
    return this;
}

__declspec(weak) void ISceneNode::BeginUpdate(float) {
}

__declspec(weak) void ISceneNode::UpdateAI(float) {
}

__declspec(weak) void ISceneNode::CommitAI() {
}

__declspec(weak) void ISceneNode::ConstrainVelocity() {
}

__declspec(weak) void ISceneNode::AttemptUpdate(float) {
}

__declspec(weak) void ISceneNode::OnCollision(const CCollision&) {
}

__declspec(weak) void ISceneNode::CommitUpdate() {
}

__declspec(weak) void ISceneNode::Draw(CDrawContext&) {
}

__declspec(weak) IVolume* ISceneNode::GetLocalBoundingVolume(EVolumeType) const {
    return 0;
}

__declspec(weak) IVolume* ISceneNode::GetWorldBoundingVolume(EVolumeType) const {
    return 0;
}

__declspec(weak) void ISceneNode::GetTMLocalToWorld(CMatrix& matrix) const {
    matrix.Ident();
}

__declspec(weak) void ISceneNode::GetPosition(CVector3& position) const {
    position.x = position.y = position.z = 0.0f;
}

__declspec(weak) void ISceneNode::GetRightward(CVector3& direction) const {
    direction.Set(1.0f, 0.0f, 0.0f);
}

__declspec(weak) void ISceneNode::GetForward(CVector3& direction) const {
    direction.Set(0.0f, 1.0f, 0.0f);
}

__declspec(weak) void ISceneNode::GetUpward(CVector3& direction) const {
    direction.Set(0.0f, 0.0f, 1.0f);
}
