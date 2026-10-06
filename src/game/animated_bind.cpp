// A fragment of animated.cpp (0x80060c50): CAnimated's binding helpers.
// BindSkeleton and BindMesh bind through the cluster and render objects and
// keep the pointer; SetAnimSpeed sets a channel's speed (136-byte channels);
// AdjustRootTrans adds a vector to the root translation; SetGeomNodeState
// forwards to the geometry state; BindSleeveTexture sets the texture of every
// mesh material flagged 0x200, and BindTexture sets one material's texture
// (or all of them for a negative index). The file name is this project's; the
// original record is animated.cpp and Draw after these is not reconstructed.
// The classes and functions are named by the mangled symbols; CAnimated and
// the mesh and material views are inferred (members at their offsets, names
// not original).
class CVector3 {
public:
    float x;
    float y;
    float z;
    float w;
} __attribute__((aligned(8)));

class CTexture;
struct CharSkel_t;
struct DMClusterObject_T {
    unsigned char unknown00[16];
};
struct DMRenderObject_T {
    unsigned char unknown00[20];
};
struct DMGeometryState_T {
    unsigned char unknown00[588];
};
struct AnimChannelView {
    unsigned char unknown00[136];
};

struct CDMeshMaterialView {
    unsigned char unknown00[36];
    unsigned int flags;
    CTexture* texture;
    unsigned char unknown2c[12];
};

class CDMesh {
public:
    unsigned char unknown00[40];
    unsigned int materialCount;
    CDMeshMaterialView* materials;
};

struct AnimRootView {
    unsigned char unknown00[8];
    float x;
    float y;
    float z;
};

void DMClusterBindSkeleton(DMClusterObject_T*, CharSkel_t*);
void DMRenderBindModel(DMRenderObject_T*, CDMesh*);
void DMGeomSetNodeState(DMGeometryState_T*, long, long);
extern "C" void AnimChanSetSpeed(AnimChannelView*, float);

class CAnimated {
public:
    void BindSkeleton(CharSkel_t*);
    void SetAnimSpeed(unsigned char, float);
    void AdjustRootTrans(CVector3);
    void SetGeomNodeState(long, long);
    void BindSleeveTexture(CTexture*);
    void BindTexture(CTexture*, char);
    void BindMesh(CDMesh*);

    unsigned char unknown0000[16];
    AnimChannelView m_channels[19];
    unsigned char unknownA28[24];
    DMRenderObject_T m_render;
    DMGeometryState_T m_geometry;
    DMClusterObject_T m_cluster;
    CDMesh* m_mesh;
    CharSkel_t* m_skeleton;
    unsigned char unknownCB8[4];
    AnimRootView* m_root;
};

void CAnimated::BindSkeleton(CharSkel_t* skeleton) {
    DMClusterBindSkeleton(&m_cluster, skeleton);
    m_skeleton = skeleton;
}

void CAnimated::SetAnimSpeed(unsigned char channel, float speed) {
    AnimChanSetSpeed(&m_channels[channel], speed);
}

void CAnimated::AdjustRootTrans(CVector3 delta) {
    AnimRootView* root = m_root;
    root->x += delta.x;
    root->y += delta.y;
    root->z += delta.z;
}

void CAnimated::SetGeomNodeState(long node, long state) {
    DMGeomSetNodeState(&m_geometry, node, state);
}

void CAnimated::BindSleeveTexture(CTexture* texture) {
    if (!m_mesh)
        return;
    for (unsigned int i = 0; i < m_mesh->materialCount; i++) {
        if (m_mesh->materials[i].flags & 0x200)
            m_mesh->materials[i].texture = texture;
    }
}

void CAnimated::BindTexture(CTexture* texture, char index) {
    if (!m_mesh)
        return;
    if (index >= 0) {
        m_mesh->materials[index].texture = texture;
        return;
    }
    for (unsigned int i = 0; i < m_mesh->materialCount; i++)
        m_mesh->materials[i].texture = texture;
}

void CAnimated::BindMesh(CDMesh* mesh) {
    DMRenderBindModel(&m_render, mesh);
    m_mesh = mesh;
}
