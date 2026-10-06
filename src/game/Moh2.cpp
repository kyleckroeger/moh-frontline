// Inline bodies of the scene-node interfaces emitted in Moh2.cpp: the
// ISceneNode defaults from IsVisible to AsMovingNode (queries returning false,
// zero or -1, empty event handlers and the null identity casts) and
// IDestructible::Destroy. They are inline in the original (weak symbols), so
// they are defined __declspec(weak). The interfaces are non-virtual views
// here; ISceneNode, IDestructible, EClsnId and the class names in the casts
// come from the mangled symbols, while the result types of the casts and
// accessors are inferred (pointers to the named classes). The rest of
// Moh2.cpp is not part of this unit.

class CAIDoodad;
class CAnimObject;
class CAnimatedPlayerObject;
class CBullet;
class CCollision;
class CCollisionVolume;
class CDrawContext;
class CHierObject;
class CLight;
class CPlayerObject;
class CPlayerWeaponObject;
class CSoldierObject;
class CStaticObject;
class CWorldObject;
class IMovingSceneNode;
struct BSObject;
enum EClsnId {};

class CVector3 {
public:
    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class ISceneNode {
public:
    bool IsVisible(CDrawContext&) const;
    bool IsDrawEnabled() const;
    EClsnId GetCollisionId() const;
    void SetCollisionId(EClsnId);
    BSObject* GetScriptObject() const;
    void TriggerScriptEvent(int, void*, bool);
    void HandleBulletCollision(CBullet*, const CCollision&);
    const CStaticObject* AsStaticObject() const;
    CStaticObject* AsStaticObject();
    const CHierObject* AsHierObject() const;
    CHierObject* AsHierObject();
    const CWorldObject* AsWorldObject() const;
    CWorldObject* AsWorldObject();
    const CAnimObject* AsAnimObject() const;
    CAnimObject* AsAnimObject();
    const CSoldierObject* AsSoldierObject() const;
    CSoldierObject* AsSoldierObject();
    const CPlayerObject* AsPlayerObject() const;
    CPlayerObject* AsPlayerObject();
    const CPlayerWeaponObject* AsPlayerWeaponObject() const;
    CPlayerWeaponObject* AsPlayerWeaponObject();
    const CAnimatedPlayerObject* AsAnimatedPlayerObject() const;
    CAnimatedPlayerObject* AsAnimatedPlayerObject();
    const CLight* AsLight() const;
    CLight* AsLight();
    const CBullet* AsBullet() const;
    CBullet* AsBullet();
    const CCollisionVolume* AsCollisionVolume() const;
    CCollisionVolume* AsCollisionVolume();
    const CAIDoodad* GetAIDoodad() const;
    CAIDoodad* GetAIDoodad();
    void SetAttachedLight(CLight*, CVector3);
    CLight* GetAttachedLight() const;
    const IMovingSceneNode* AsMovingNode() const;
    IMovingSceneNode* AsMovingNode();
};

class IDestructible {
public:
    void Destroy();
};

__declspec(weak) bool ISceneNode::IsVisible(CDrawContext&) const {
    return false;
}

__declspec(weak) bool ISceneNode::IsDrawEnabled() const {
    return false;
}

__declspec(weak) EClsnId ISceneNode::GetCollisionId() const {
    return (EClsnId)-1;
}

__declspec(weak) void ISceneNode::SetCollisionId(EClsnId) {
}

__declspec(weak) BSObject* ISceneNode::GetScriptObject() const {
    return 0;
}

__declspec(weak) void ISceneNode::TriggerScriptEvent(int, void*, bool) {
}

__declspec(weak) void ISceneNode::HandleBulletCollision(CBullet*, const CCollision&) {
}

__declspec(weak) const CStaticObject* ISceneNode::AsStaticObject() const {
    return 0;
}

__declspec(weak) CStaticObject* ISceneNode::AsStaticObject() {
    return 0;
}

__declspec(weak) const CHierObject* ISceneNode::AsHierObject() const {
    return 0;
}

__declspec(weak) CHierObject* ISceneNode::AsHierObject() {
    return 0;
}

__declspec(weak) const CWorldObject* ISceneNode::AsWorldObject() const {
    return 0;
}

__declspec(weak) CWorldObject* ISceneNode::AsWorldObject() {
    return 0;
}

__declspec(weak) const CAnimObject* ISceneNode::AsAnimObject() const {
    return 0;
}

__declspec(weak) CAnimObject* ISceneNode::AsAnimObject() {
    return 0;
}

__declspec(weak) const CSoldierObject* ISceneNode::AsSoldierObject() const {
    return 0;
}

__declspec(weak) CSoldierObject* ISceneNode::AsSoldierObject() {
    return 0;
}

__declspec(weak) const CPlayerObject* ISceneNode::AsPlayerObject() const {
    return 0;
}

__declspec(weak) CPlayerObject* ISceneNode::AsPlayerObject() {
    return 0;
}

__declspec(weak) const CPlayerWeaponObject* ISceneNode::AsPlayerWeaponObject() const {
    return 0;
}

__declspec(weak) CPlayerWeaponObject* ISceneNode::AsPlayerWeaponObject() {
    return 0;
}

__declspec(weak) const CAnimatedPlayerObject* ISceneNode::AsAnimatedPlayerObject() const {
    return 0;
}

__declspec(weak) CAnimatedPlayerObject* ISceneNode::AsAnimatedPlayerObject() {
    return 0;
}

__declspec(weak) const CLight* ISceneNode::AsLight() const {
    return 0;
}

__declspec(weak) CLight* ISceneNode::AsLight() {
    return 0;
}

__declspec(weak) const CBullet* ISceneNode::AsBullet() const {
    return 0;
}

__declspec(weak) CBullet* ISceneNode::AsBullet() {
    return 0;
}

__declspec(weak) const CCollisionVolume* ISceneNode::AsCollisionVolume() const {
    return 0;
}

__declspec(weak) CCollisionVolume* ISceneNode::AsCollisionVolume() {
    return 0;
}

__declspec(weak) const CAIDoodad* ISceneNode::GetAIDoodad() const {
    return 0;
}

__declspec(weak) CAIDoodad* ISceneNode::GetAIDoodad() {
    return 0;
}

__declspec(weak) void ISceneNode::SetAttachedLight(CLight*, CVector3) {
}

__declspec(weak) CLight* ISceneNode::GetAttachedLight() const {
    return 0;
}

__declspec(weak) void IDestructible::Destroy() {
}

__declspec(weak) const IMovingSceneNode* ISceneNode::AsMovingNode() const {
    return 0;
}

__declspec(weak) IMovingSceneNode* ISceneNode::AsMovingNode() {
    return 0;
}
