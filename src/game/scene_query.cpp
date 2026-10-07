// A fragment of scene.cpp (0x800d8214): CScene::IsNodeInScene (a node is in
// the scene if it is the first node or the second scene pointer, or if its
// own link pointers at +8 or +4 are set) and GetPlayer (the player pointers
// from +8). CScene, ISceneNode and CPlayerObject are named by the mangled
// symbols; the members are inferred views (CScene is non-virtual; see
// scene.cpp). Deferred inlining lists the functions in reverse. The rest of the file is not part of this unit.
class CPlayerObject;

class ISceneNode {
public:
    unsigned char unknown0[4];
    ISceneNode* m_link4;
    ISceneNode* m_link8;
};

class CScene {
public:
    bool IsNodeInScene(const ISceneNode*) const;
    CPlayerObject* GetPlayer(int) const;

    ISceneNode* m_firstNode;
    ISceneNode* m_node4;
    CPlayerObject* m_players[4];
};

CPlayerObject* CScene::GetPlayer(int index) const {
    return m_players[index];
}

bool CScene::IsNodeInScene(const ISceneNode* node) const {
    if (!node)
        return false;
    if (m_firstNode == node)
        return true;
    if (m_node4 == node)
        return true;
    if (node->m_link8)
        return true;
    return node->m_link4 != 0;
}
