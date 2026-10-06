// CHearingVolume, the functions at the start of the file: the weak collision
// id accessors and the bullet whiz-by sound, played at the bullet's position
// and heading for the scene's first player. The class names come from the
// mangled symbols. CHearingVolume is an inferred, non-virtual view (its
// virtual table is not reproduced); CBullet is declared with the ISceneNode
// virtual functions in the order of __vt__10ISceneNode, flattened, and
// CVector3 is the game's 16-byte, 8-aligned vector (copied by value). The
// bullet handler that follows is not reconstructed.
class CDrawContext;
class CCollision;
class CMatrix;
class CPlayerObject;
class IMovingSceneNode;

enum EClsnId {};

class CVector3 {
public:
    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CBullet {
public:
    enum EVolumeType {};

    virtual void MarkForDestruction(int);
    virtual ~CBullet();
    virtual void Destroy();
    virtual void BeginUpdate(float);
    virtual void UpdateAI(float);
    virtual void CommitAI();
    virtual void ConstrainVelocity();
    virtual void AttemptUpdate(float);
    virtual void OnCollision(const CCollision&);
    virtual void CommitUpdate();
    virtual void Draw(CDrawContext&);
    virtual const void* GetLocalBoundingVolume(EVolumeType) const;
    virtual const void* GetWorldBoundingVolume(EVolumeType) const;
    virtual void GetTMLocalToWorld(CMatrix&) const;
    virtual void GetPosition(CVector3&) const;
    virtual void GetRightward(CVector3&) const;
    virtual void GetForward(CVector3&) const;
};

class CScene {
public:
    CPlayerObject* GetPlayer(int) const;

    char data[328];
};

extern CScene g_scene;

void PlayBulletWhiz(CVector3, CVector3, CPlayerObject*);

class CHearingVolume {
public:
    EClsnId GetCollisionId() const;
    void SetCollisionId(EClsnId);

    unsigned char unknown000[192];
    EClsnId m_collisionId;
};

__declspec(weak) EClsnId CHearingVolume::GetCollisionId() const {
    return m_collisionId;
}

__declspec(weak) void CHearingVolume::SetCollisionId(EClsnId id) {
    m_collisionId = id;
}

void PlayBulletWhizBys(IMovingSceneNode*, CBullet* bullet) {
    CVector3 position;
    CVector3 direction;

    bullet->GetPosition(position);
    bullet->GetForward(direction);
    PlayBulletWhiz(position, direction, g_scene.GetPlayer(0));
}
