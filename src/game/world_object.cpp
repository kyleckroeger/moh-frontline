// CWorldObject, the functions at the start of the file: identity and
// scene-node queries, and drawing the world's compartments. The class names
// come from the mangled symbols. CWorldObject is an inferred, non-virtual
// view: only the members these functions touch are declared, at their offsets
// (their names are not original), and its virtual table is not reproduced.
// The compartments sit in a linked list whose nodes hold a CCompartment after
// the two links (the list view is inferred). CCompartment is declared with the
// ISceneNode virtual functions in the order of __vt__10ISceneNode, flattened
// into one class; only the calls made here matter.
class CDrawContext;
class CCollision;
class CMatrix;
class CVector3;

class CCompartment {
public:
    enum EVolumeType {};

    virtual void MarkForDestruction(int);
    virtual ~CCompartment();
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
    virtual void GetUpward(CVector3&) const;
    virtual bool IsVisible(CDrawContext&) const;
    virtual bool IsDrawEnabled() const;

    void DrawShadows(CDrawContext&);
};

struct WorldCompartmentNodeView {
    WorldCompartmentNodeView* prev;
    WorldCompartmentNodeView* next;
    CCompartment compartment;
};

struct WorldListLinkView {
    WorldCompartmentNodeView* prev;
    WorldCompartmentNodeView* next;
};

class ISceneNode {
public:
    enum EVolumeType {};
};

class CWorldObject {
public:
    const CWorldObject* AsWorldObject() const;
    CWorldObject* AsWorldObject();
    int GetCollisionId() const;
    bool IsDrawEnabled() const;
    bool IsVisible(CDrawContext&) const;
    const void* GetWorldBoundingVolume(ISceneNode::EVolumeType) const;
    const void* GetLocalBoundingVolume(ISceneNode::EVolumeType) const;
    void DrawShadows(CDrawContext&);
    void Draw(CDrawContext&);

    unsigned char unknown000[80];
    WorldListLinkView m_compartments;
    unsigned char unknown058[184];
    unsigned char m_boundingVolume[80];
    bool m_drawEnabled;
};

__declspec(weak) const CWorldObject* CWorldObject::AsWorldObject() const {
    return this;
}

__declspec(weak) CWorldObject* CWorldObject::AsWorldObject() {
    return this;
}

int CWorldObject::GetCollisionId() const {
    return 0;
}

bool CWorldObject::IsDrawEnabled() const {
    return m_drawEnabled;
}

bool CWorldObject::IsVisible(CDrawContext&) const {
    return true;
}

const void* CWorldObject::GetWorldBoundingVolume(ISceneNode::EVolumeType) const {
    return m_boundingVolume;
}

const void* CWorldObject::GetLocalBoundingVolume(ISceneNode::EVolumeType) const {
    return m_boundingVolume;
}

void CWorldObject::DrawShadows(CDrawContext& context) {
    WorldCompartmentNodeView* node = m_compartments.next;
    WorldCompartmentNodeView* end = (WorldCompartmentNodeView*)&m_compartments;
    for (; node != end; node = node->next) {
        if (node->compartment.IsDrawEnabled() && node->compartment.IsVisible(context))
            node->compartment.DrawShadows(context);
    }
}

void CWorldObject::Draw(CDrawContext& context) {
    WorldCompartmentNodeView* node = m_compartments.next;
    WorldCompartmentNodeView* end = (WorldCompartmentNodeView*)&m_compartments;
    for (; node != end; node = node->next) {
        if (node->compartment.IsDrawEnabled() && node->compartment.IsVisible(context))
            node->compartment.Draw(context);
    }
}
