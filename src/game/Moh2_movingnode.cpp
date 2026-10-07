// A fragment of Moh2.cpp (0x8001ae48): IMovingSceneNode's empty default
// movement functions, weak copies of header inlines emitted in this file (so
// they are defined __declspec(weak)), and GetWorldLinearVelocity (zero; its
// constant is an entry of the file's .sdata2 pool). IMovingSceneNode, CVector3
// and CMatrix are named by the mangled symbols; IMovingSceneNode is a
// non-virtual view and the CVector3 constructor is inferred. The rest of the
// file is not part of this unit.
class CMatrix;

class CVector3 {
public:
    CVector3() {}
    CVector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

    float x;
    float y;
    float z;
} __attribute__((aligned(8)));

class IMovingSceneNode {
public:
    void Reset();
    void Halt();
    void ApplyForceTo(CVector3, float);
    void SetTMLocalToWorld(const CMatrix&);
    void PreTransform(const CMatrix&);
    void Transform(const CMatrix&);
    void Move(CVector3);
    void Rotate(CVector3, float);
    void Pitch(float);
    void Roll(float);
    void Yaw(float);
    void SetPosition(CVector3);
    void SetBasis(CVector3, CVector3, CVector3);
    void Orthonormalize();
    CVector3 GetWorldLinearVelocity() const;
};

__declspec(weak) void IMovingSceneNode::Reset() {
}

__declspec(weak) void IMovingSceneNode::Halt() {
}

__declspec(weak) void IMovingSceneNode::ApplyForceTo(CVector3, float) {
}

__declspec(weak) void IMovingSceneNode::SetTMLocalToWorld(const CMatrix&) {
}

__declspec(weak) void IMovingSceneNode::PreTransform(const CMatrix&) {
}

__declspec(weak) void IMovingSceneNode::Transform(const CMatrix&) {
}

__declspec(weak) void IMovingSceneNode::Move(CVector3) {
}

__declspec(weak) void IMovingSceneNode::Rotate(CVector3, float) {
}

__declspec(weak) void IMovingSceneNode::Pitch(float) {
}

__declspec(weak) void IMovingSceneNode::Roll(float) {
}

__declspec(weak) void IMovingSceneNode::Yaw(float) {
}

__declspec(weak) void IMovingSceneNode::SetPosition(CVector3) {
}

__declspec(weak) void IMovingSceneNode::SetBasis(CVector3, CVector3, CVector3) {
}

__declspec(weak) void IMovingSceneNode::Orthonormalize() {
}

__declspec(weak) CVector3 IMovingSceneNode::GetWorldLinearVelocity() const {
    return CVector3(0.0f, 0.0f, 0.0f);
}
