// Search-node queue: a binary min-heap of search nodes ordered by cost, with
// the element count in the first word and the 1-based heap entries after it
// (so entry i is the word at index i); each node records its heap index.
// CSearchNodeList is a CLinkedList with nothing of its own. The class names
// come from the mangled symbols; the members and the heap view are inferred.
class CSearchNode {
public:
    ~CSearchNode();

    int m_heapIndex;
    unsigned char unknown04[16];
    float m_cost;
};

class CSearchNodeQueue {
public:
    void Clear();
    CSearchNode* PopFirst();
    void ReplaceElement(CSearchNode*, CSearchNode*);
    void Insert(CSearchNode*);

    int m_count;
    CSearchNode* m_nodes[1];
};

#define HEAP(i) (m_nodes[(i) - 1])

void CSearchNodeQueue::Clear() {
    for (int i = 1; i < m_count; i++) {
        HEAP(i)->m_heapIndex = 0;
        delete HEAP(i);
    }
    m_count = 1;
}

CSearchNode* CSearchNodeQueue::PopFirst() {
    int next;
    CSearchNode* last;
    int hole;
    int child;
    CSearchNode* first;

    if (m_count <= 1)
        return 0;
    first = HEAP(1);
    last = HEAP(--m_count);
    hole = 1;
    while ((child = hole * 2) < m_count) {
        next = child + 1;
        if (next < m_count && HEAP(child + 1)->m_cost < HEAP(child)->m_cost)
            child = next;
        if (last->m_cost < HEAP(child)->m_cost)
            break;
        HEAP(hole) = HEAP(child);
        HEAP(hole)->m_heapIndex = hole;
        hole = child;
    }
    HEAP(hole) = last;
    last->m_heapIndex = hole;
    first->m_heapIndex = 0;
    return first;
}

void CSearchNodeQueue::ReplaceElement(CSearchNode* old, CSearchNode* node) {
    int parent;
    int i;
    int next;
    CSearchNode* last;
    int hole;
    int child;

    hole = old->m_heapIndex;
    last = HEAP(--m_count);
    while ((child = hole * 2) < m_count) {
        next = child + 1;
        if (next < m_count && HEAP(child + 1)->m_cost < HEAP(child)->m_cost)
            child = next;
        if (last->m_cost < HEAP(child)->m_cost)
            break;
        HEAP(hole) = HEAP(child);
        HEAP(hole)->m_heapIndex = hole;
        hole = child;
    }
    HEAP(hole) = last;
    last->m_heapIndex = hole;

    i = m_count;
    m_count++;
    parent = i >> 1;
    while (i > 1 && node->m_cost < HEAP(parent)->m_cost) {
        HEAP(i) = HEAP(parent);
        HEAP(i)->m_heapIndex = i;
        i = parent;
        parent >>= 1;
    }
    HEAP(i) = node;
    node->m_heapIndex = i;
    old->m_heapIndex = 0;
}

void CSearchNodeQueue::Insert(CSearchNode* node) {
    int i = m_count++;
    int parent = i >> 1;
    while (i > 1 && node->m_cost < HEAP(parent)->m_cost) {
        HEAP(i) = HEAP(parent);
        HEAP(i)->m_heapIndex = i;
        i = parent;
        parent >>= 1;
    }
    HEAP(i) = node;
    node->m_heapIndex = i;
}

class CLinkedList {
public:
    CLinkedList();
    ~CLinkedList();
};

class CSearchNodeList : public CLinkedList {
public:
    CSearchNodeList();
    ~CSearchNodeList();
};

CSearchNodeList::~CSearchNodeList() {
}

CSearchNodeList::CSearchNodeList() {
}
