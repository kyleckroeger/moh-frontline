// A fragment of AIObject.cpp (0x8004b728): CAIObject::CleanupAStarPathWalk,
// deleting the A* search nodes held in the list at +600 (inferred from offsets
// in a non-virtual view).
class CLinkedListElement;

class CLinkedList {
public:
    void RemoveElement(CLinkedListElement*);

    CLinkedListElement* m_head;
};

class CLinkedListElement {
public:
    unsigned char unknown00[4];
};

class CSearchNode : public CLinkedListElement {
public:
    ~CSearchNode();
};

class CAIObject {
public:
    void CleanupAStarPathWalk();

    unsigned char unknown000[600];
    CLinkedList m_searchNodes;
};

void CAIObject::CleanupAStarPathWalk() {
    CSearchNode* node = (CSearchNode*)m_searchNodes.m_head;
    while (node) {
        m_searchNodes.RemoveElement(node);
        delete node;
        node = (CSearchNode*)m_searchNodes.m_head;
    }
}
